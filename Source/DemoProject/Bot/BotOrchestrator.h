#pragma once

#include "AgentModule.h"
#include "Bot/Factories/BotFactory.h"
#include "BotOrchestrator.generated.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

/**
 * FBotInstance - Managed data for a single AI Bot entity.
 * Bridges the physical Actor, the Functional State Store, and the SDK Agent.
 */
struct FBotInstance {
  AActor *BotActor;
  TSharedPtr<const FAgent> Agent;
  ForbocAI::Bot::FBotStore Store;
  float LastObservationTime;

  FBotInstance()
      : BotActor(nullptr), Agent(nullptr), Store({}),
        LastObservationTime(0.0f) {}
};

/**
 * ABotOrchestrator - The central brain for the Demo's AI entities.
 * Implements the 7-step protocol loop asynchronously for all registered bots.
 */
UCLASS()
class DEMOPROJECT_API ABotOrchestrator : public AActor {
  GENERATED_BODY()

public:
  ABotOrchestrator();

protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

public:
  /** Configuration: How often (in seconds) bots observe the world. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForbocAI")
  float ObservationInterval = 5.0f;

  /** API URL for the SDK. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForbocAI")
  FString ApiUrl = TEXT("http://localhost:8080");

  /** Register a physical actor as a managed bot. */
  UFUNCTION(BlueprintCallable, Category = "ForbocAI")
  void RegisterBot(AActor *Actor, FString Persona);

private:
  /** Internal registry of active bots. */
  TMap<AActor *, FBotInstance> ActiveBots;

  /** 7-Step Protocol: Stage 1 (Observe & Process) */
  void RequestNextAction(FBotInstance &Instance);

  /** 7-Step Protocol: Stage 7 (Execute) */
  void ExecuteAction(AActor *BotActor, const FAgentAction &Action);

  /** Helper to map game state to strings for observation. */
  FString GetStateObservation(const ForbocAI::State::FBotState &State);
};
