#include "BotOrchestrator.h"
#include "State/Actions.h"

ABotOrchestrator::ABotOrchestrator() { PrimaryActorTick.bCanEverTick = true; }

void ABotOrchestrator::BeginPlay() {
  Super::BeginPlay();
  UE_LOG(LogTemp, Display, TEXT("BotOrchestrator: Brain Online."));
}

void ABotOrchestrator::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  float CurrentTime = GetWorld()->GetTimeSeconds();

  for (auto &Pair : ActiveBots) {
    FBotInstance &Instance = Pair.Value;

    // 1. Functional Store Tick (Heartbeat)
    ForbocAI::State::FActionTick TickAction;
    TickAction.DeltaTime = DeltaTime;
    Instance.Store.Dispatch(TickAction);

    // 2. Observation Logic (Interval-based)
    if (CurrentTime - Instance.LastObservationTime >= ObservationInterval) {
      Instance.LastObservationTime = CurrentTime;
      RequestNextAction(Instance);
    }
  }
}

void ABotOrchestrator::RegisterBot(AActor *Actor, FString Persona) {
  if (!Actor)
    return;

  FBotInstance Instance;
  Instance.BotActor = Actor;

  // Initialize Functional Store
  Instance.Store = ForbocAI::Bot::Factory::CreateBotStore(Actor->GetName());

  // Initialize SDK Agent
  FAgentConfig Config;
  Config.Persona = Persona;
  Config.ApiUrl = ApiUrl;

  auto AgentResult = AgentFactory::Create(Config);
  if (AgentResult.isRight) {
    Instance.Agent = MakeShared<const FAgent>(AgentResult.right);
    ActiveBots.Add(Actor, Instance);
    UE_LOG(LogTemp, Display, TEXT("BotOrchestrator: Registered Bot '%s'"),
           *Actor->GetName());
  } else {
    UE_LOG(LogTemp, Error, TEXT("BotOrchestrator: Failed to create agent: %s"),
           *AgentResult.left);
  }
}

void ABotOrchestrator::RequestNextAction(FBotInstance &Instance) {
  if (!Instance.Agent.IsValid())
    return;

  // Step 1: OBSERVE
  // Combine internal functional state with physical world state
  ForbocAI::State::FBotState InternalState = Instance.Store.GetState();
  FString Observation = GetStateObservation(InternalState);

  // Step 2-6: Protocol Pipeline (Directive -> Generate -> Verdict)
  // Re-captured for safety in the async lambda
  AActor *BotActor = Instance.BotActor;

  AgentOps::Process(*Instance.Agent, Observation, {},
                    [this, BotActor](FAgentResponse Response) {
                      // Step 7: EXECUTE
                      ExecuteAction(BotActor, Response.Action);
                    });
}

void ABotOrchestrator::ExecuteAction(AActor *BotActor,
                                     const FAgentAction &Action) {
  if (!BotActor || !ActiveBots.Contains(BotActor))
    return;

  FBotInstance &Instance = ActiveBots[BotActor];

  UE_LOG(LogTemp, Display, TEXT("BotOrchestrator: Executing '%s' for %s"),
         *Action.Type, *BotActor->GetName());

  // Map SDK Action -> Functional Action -> Dispatch to Store
  if (Action.Type == TEXT("MOVE")) {
    ForbocAI::State::FActionMove Move;
    // Simple mock: Move to target if specified in target field
    // In a real game, would parse the observation context or payload
    Move.TargetLocation = BotActor->GetActorLocation() + FVector(500, 0, 0);
    Move.Speed = 100.0f;
    Instance.Store.Dispatch(Move);
  } else if (Action.Type == TEXT("ATTACK")) {
    ForbocAI::State::FActionAttack Attack;
    Instance.Store.Dispatch(Attack);
  }
  // ... and so on
}

FString
ABotOrchestrator::GetStateObservation(const ForbocAI::State::FBotState &State) {
  return FString::Printf(
      TEXT("Name: %s, Health: %.1f, Position: %s, Phase: %d"), *State.Name,
      State.Stats.Health, *State.Position.ToString(), (int32)State.Phase);
}
