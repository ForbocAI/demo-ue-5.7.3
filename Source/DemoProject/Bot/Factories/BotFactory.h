#pragma once

#include "State/Actions.h"
#include "State/BotState.h"
#include "State/Reducers.h"
#include <functional>

namespace ForbocAI {
namespace Bot {

// ── The Store Type ──
// A Store is simply a function that takes an Action and returns the new State.
// However, to maintain state *across* calls, we need a closure or an object.
// Since we can't use classes, we'll use a functional closure approach
// where the State is held in a shared_ptr captured by the lambda.

using Dispatcher = std::function<State::FBotState(State::FBotAction)>;
using StateGetter = std::function<State::FBotState()>;

struct FBotStore {
  Dispatcher Dispatch;
  StateGetter GetState;
};

namespace Factory {

inline FBotStore CreateBotStore(const FString &BotName) {
  // Shared State held by the closure
  // We use std::make_shared to ensure the state survives after this function
  // returns
  auto StateContainer =
      std::make_shared<State::FBotState>(State::CreateInitialState(BotName));

  Dispatcher Dispatch =
      [StateContainer](State::FBotAction Action) -> State::FBotState {
    // 1. Reduce
    State::FBotState NewState = State::Reduce(*StateContainer, Action);

    // 2. Update (Mutation of the container, effectively "State = NewState")
    *StateContainer = NewState;

    return NewState;
  };

  StateGetter GetState = [StateContainer]() -> State::FBotState {
    return *StateContainer;
  };

  return {Dispatch, GetState};
}
} // namespace Factory

} // namespace Bot
} // namespace ForbocAI
