#pragma once

#include "CoreMinimal.h"
#include <variant>

namespace ForbocAI {
namespace State {

// ── Action Payloads ──

struct FActionTick {
  float DeltaTime;
};

struct FActionMove {
  FVector TargetLocation;
  float Speed;
};

struct FActionTakeDamage {
  float Amount;
  AActor *Source;
};

struct FActionSpotEnemy {
  FVector EnemyLocation;
};

struct FActionAttack {
  AActor *Target;
};

struct FActionFlee {
  FVector AwayFrom;
};

// ── Action Variant (Sum Type) ──

// The set of all possible actions the reducer can handle
using FBotAction = std::variant<FActionTick, FActionMove, FActionTakeDamage,
                                FActionSpotEnemy, FActionAttack, FActionFlee>;

} // namespace State
} // namespace ForbocAI
