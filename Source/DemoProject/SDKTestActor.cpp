
#include "SDKTestActor.h"
#include "AgentModule.h"
#include "BridgeModule.h"
#include "MemoryModule.h"
#include "SoulModule.h"

ASDKTestActor::ASDKTestActor() {
  // No tick needed — this actor responds to events only.
  PrimaryActorTick.bCanEverTick = false;

  Persona = TEXT("Cyber-Merchant");
  ApiUrl = TEXT("https://api.forboc.ai");
}

void ASDKTestActor::BeginPlay() {
  Super::BeginPlay();

  // Auto-initialize if a Persona is configured
  if (Persona.Len() > 0) {
    InitializeAgent();

    UE_LOG(LogTemp, Display, TEXT("SDKTestActor: Auto-initialized agent %s"),
           *CurrentAgent->Id);
  }
}

void ASDKTestActor::InitializeAgent() {
  FAgentConfig Config;
  Config.Persona = Persona;
  Config.ApiUrl = ApiUrl;

  // Create agent via factory function (Functional C++ pattern).
  // MakeShared wraps the immutable FAgent so we can rebind later.
  CurrentAgent = MakeShared<const FAgent>(AgentFactory::Create(Config));

  // Manually register RPG rules (formerly default) via Preset
  ActiveRules = BridgeOps::CreateRPGRules();

  UE_LOG(LogTemp, Display, TEXT("ForbocAI: Created Agent with Persona '%s'"),
         *Persona);
  UE_LOG(LogTemp, Display,
         TEXT("ForbocAI: Registered %d validation rules via RPG Preset"),
         ActiveRules.Num());

  // REGISTER RULES WITH API (New Feature)
  for (const FValidationRule &Rule : ActiveRules) {
    BridgeOps::RegisterRule(Rule, ApiUrl);
  }

  // Trigger Blueprint event
  OnAgentInitialized(CurrentAgent->Id);
}

void ASDKTestActor::ProcessInput(const FString &InputText) {
  if (!CurrentAgent.IsValid()) {
    UE_LOG(LogTemp, Warning,
           TEXT("ForbocAI: Cannot process input, agent not initialized."));
    return;
  }

  // Process input via async pipeline.
  AgentOps::Process(
      *CurrentAgent, InputText, {}, [this](FAgentResponse Response) {
        // ==========================================
        // BRIDGE: Validate the Agent's Action
        // ==========================================
        // We use the "ActiveRules" we manually registered.
        // Context would typically include World State.
        const FBridgeValidationContext ValContext =
            BridgeFactory::CreateContext(&CurrentAgent->State, {});

        const FValidationResult ValResult =
            BridgeOps::Validate(Response.Action, ActiveRules, ValContext);

        if (ValResult.bValid) {
          UE_LOG(LogTemp, Display, TEXT("Bridge: Action VALID (%s)"),
                 *ValResult.Reason);
        } else {
          UE_LOG(LogTemp, Warning, TEXT("Bridge: Action BLOCKED (%s)"),
                 *ValResult.Reason);
          // In a real game, we might override the response or prevent execution
          // here.
        }

        UE_LOG(LogTemp, Display, TEXT("ForbocAI Response: %s"),
               *Response.Dialogue);

        // Trigger Blueprint event
        OnAgentResponse(Response.Dialogue);
      });
}

void ASDKTestActor::UpdateAgentState(const FString &NewStateDescription) {
  if (!CurrentAgent.IsValid())
    return;

  // Functional update — returns a NEW agent. The old agent
  // data remains untouched; we rebind the shared pointer.

  // Wrap the description in a simple JSON structure for the generic state
  const FString JsonState =
      FString::Printf(TEXT("{\"description\": \"%s\"}"), *NewStateDescription);

  const FAgentState NewState = TypeFactory::AgentState(JsonState);
  CurrentAgent =
      MakeShared<const FAgent>(AgentOps::WithState(*CurrentAgent, NewState));

  UE_LOG(LogTemp, Display, TEXT("ForbocAI: Updated Agent State to '%s'"),
         *JsonState);
}

void ASDKTestActor::ExportSoul() {
  if (!CurrentAgent.IsValid()) {
    UE_LOG(LogTemp, Warning,
           TEXT("ForbocAI: Cannot export Soul, agent not initialized."));
    return;
  }

  // Create Soul from current Agent State
  // Note: We pass empty memories for demo simplicity, or fetch from
  // MemoryModule if implemented.
  FSoul Soul = SoulOps::FromAgent(CurrentAgent->State, {}, CurrentAgent->Id,
                                  CurrentAgent->Persona);

  UE_LOG(LogTemp, Display, TEXT("ForbocAI: Exporting Soul to Arweave..."));

  // Call SDK Ops
  SoulOps::ExportToArweave(Soul, ApiUrl, [this](FString TxId) {
    // Logic inside callback (on game thread via Lambda, ensure thread safety if
    // needed) Getting back on Game Thread usually handled by HTTP module
    // callbacks.
    UE_LOG(LogTemp, Display, TEXT("ForbocAI: Soul Exported! TxId: %s"), *TxId);
    OnSoulExported(TxId);
  });
}
