
#pragma once

#include "AgentModule.h"  // ForbocAI SDK
#include "BridgeModule.h" // Validation Rules
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDKTestActor.generated.h"

/**
 * Demo Actor — ForbocAI SDK integration example.
 *
 * This is a UE boundary layer: AActors require UCLASS, a
 * constructor, and virtual overrides. These are UE framework
 * obligations, not violations of the FP architecture.
 *
 * The SDK itself (FAgent, AgentFactory, AgentOps, etc.)
 * follows strict FP: pure data structs, factory functions,
 * and free functions only.
 *
 * Because FAgent has const members (immutable data), it cannot
 * be reassigned with operator=. We wrap it in TSharedPtr<const FAgent>
 * to allow rebinding the pointer while the underlying data stays
 * immutable — the standard FP pattern for mutable references
 * to immutable values.
 */
UCLASS()
class DEMOPROJECT_API ASDKTestActor : public AActor {
  GENERATED_BODY()

public:
  ASDKTestActor();

protected:
  virtual void BeginPlay() override;

public:
  // --- Configuration (Blueprint-editable) ---

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForbocAI")
  FString Persona;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForbocAI")
  FString ApiUrl;

  // --- State ---

  /**
   * Current agent — wrapped in TSharedPtr because FAgent has
   * const members (immutable). The pointer is rebound on each
   * functional update; the data it points to is never mutated.
   */
  TSharedPtr<const FAgent> CurrentAgent;

  /**
   * Active Validation Rules.
   * Registered via BridgeOps presets.
   * Not a UPROPERTY because FValidationRule contains std::function.
   */
  TArray<FValidationRule> ActiveRules;

  // --- Blueprint Callable Functions ---

  UFUNCTION(BlueprintCallable, Category = "ForbocAI")
  void InitializeAgent();

  UFUNCTION(BlueprintCallable, Category = "ForbocAI")
  void ProcessInput(const FString &InputText);

  UFUNCTION(BlueprintCallable, Category = "ForbocAI")
  void UpdateAgentState(const FString &NewStateDescription);

  UFUNCTION(BlueprintCallable, Category = "ForbocAI")
  void ExportSoul();

  // --- Events (implement in Blueprint) ---

  UFUNCTION(BlueprintImplementableEvent, Category = "ForbocAI")
  void OnAgentResponse(const FString &ResponseText);

  UFUNCTION(BlueprintImplementableEvent, Category = "ForbocAI")
  void OnAgentInitialized(const FString &AgentId);

  UFUNCTION(BlueprintImplementableEvent, Category = "ForbocAI")
  void OnSoulExported(const FString &TxId);
};
