#pragma once

#include "Actions.h"
#include "BotState.h"
#include "Core/functional_core.hpp"

namespace ForbocAI {
namespace State {

// ── Reducer Helpers ──

// Handler for specific actions (Overload pattern)
// We use a struct with operator() because C++ templated lambdas in std::visit
// can be verbose.

struct ReducerVisitor {
  const FBotState &CurrentState;

  // 1. Tick
  FBotState operator()(const FActionTick &Action) const {
    FBotState Next = CurrentState;
    Next.TickCount++;

    // Memory Decay
    Next.Memory.TimeSinceLastSeenPlayer += Action.DeltaTime;
    if (Next.Memory.TimeSinceLastSeenPlayer > 10.0f) {
      Next.Memory.bHasAggro = false; // Lost aggro
    }

    return Next;
  }

  // 2. Move
  FBotState operator()(const FActionMove &Action) const {
    FBotState Next = CurrentState;
    // In a pure reducer, we just update the *intent* or physical state if we
    // are the authority. Here we assume the Actuator will actually move the
    // pawn, and we update our internal record. Or, if this is the "Brain"
    // state, we might just set a "Goal" field. For this example, let's assume
    // we update Position to Target for simulation (or interpolation).
    Next.Position = Action.TargetLocation;
    return Next;
  }

  // 3. Take Damage
  FBotState operator()(const FActionTakeDamage &Action) const {
    FBotState Next = CurrentState;
    Next.Stats.Health = FMath::Max(0.0f, Next.Stats.Health - Action.Amount);

    // Reaction: If hit, enter combat or flee
    if (Next.Stats.Health < Next.Stats.MaxHealth * 0.3f) {
      Next.Phase = EBotPhase::Flee;
    } else {
      Next.Phase = EBotPhase::Combat;
    }
    return Next;
  }

  // 4. Spot Enemy
  FBotState operator()(const FActionSpotEnemy &Action) const {
    FBotState Next = CurrentState;
    Next.Memory.LastKnownPlayerPos = Action.EnemyLocation;
    Next.Memory.TimeSinceLastSeenPlayer = 0.0f;
    Next.Memory.bHasAggro = true;

    if (Next.Phase != EBotPhase::Flee) {
      Next.Phase = EBotPhase::Combat;
    }
    return Next;
  }

  // 5. Default / Others
  template <typename T> FBotState operator()(const T &Action) const {
    return CurrentState; // No change for unhandled actions
  }
};

// ── Main Reducer Function ──

inline FBotState Reduce(const FBotState &State, const FBotAction &Action) {
  // We visit the action variant with our visitor.
  // The visitor MUST implement operator() for every type in the variant,
  // OR have a generic template operator().
  return std::visit(ReducerVisitor{State}, Action);
}

} // namespace State
} // namespace ForbocAI
