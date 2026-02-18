#pragma once

#include "CoreMinimal.h"
#include "functional_core.hpp"

namespace ForbocAI {
namespace State {

// ── Sub-States ──

struct FStats {
  float Health = 100.0f;
  float MaxHealth = 100.0f;
  float Mana = 100.0f;
  float MaxMana = 100.0f;
  float Stamina = 100.0f;
  float MaxStamina = 100.0f;
};

struct FMemory {
  // Minimal memory representation for now
  FVector LastKnownPlayerPos = FVector::ZeroVector;
  float TimeSinceLastSeenPlayer = 9999.0f;
  bool bHasAggro = false;
};

enum class EBotPhase { Idle, Patrol, Combat, Flee, Search };

// ── Main State ──

struct FBotState {
  FGuid Id;
  FString Name;

  FVector Position;
  FRotator Rotation;

  FStats Stats;
  FMemory Memory;

  EBotPhase Phase = EBotPhase::Idle;

  // "Redux" tick counter
  uint64 TickCount = 0;
};

// ── Initial State Factory ──

inline FBotState CreateInitialState(FString InName) {
  FBotState State;
  State.Id = FGuid::NewGuid();
  State.Name = InName;
  State.Position = FVector::ZeroVector;
  State.Rotation = FRotator::ZeroRotator;
  return State;
}

} // namespace State
} // namespace ForbocAI
