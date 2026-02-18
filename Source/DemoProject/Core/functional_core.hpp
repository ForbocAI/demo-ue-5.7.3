#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <variant>

namespace ForbocAI {
namespace Core {

// ── Maybe Monad (std::optional wrapper) ──
template <typename T> using Maybe = std::optional<T>;

template <typename T> Maybe<T> Just(T value) {
  return std::make_optional(value);
}

template <typename T> Maybe<T> Nothing() { return std::nullopt; }

// Bind (>>=) for Maybe
template <typename T, typename Func>
auto operator>>=(const Maybe<T> &m, Func f) -> decltype(f(*m)) {
  if (m.has_value()) {
    return f(*m);
  }
  return std::nullopt;
}

// ── Result Monad (std::variant wrapper) ──
struct Error {
  std::string message;
};

template <typename T> using Result = std::variant<Error, T>;

template <typename T> Result<T> Ok(T value) { return value; }

template <typename T> Result<T> Err(std::string msg) { return Error{msg}; }

// Bind (>>=) for Result
template <typename T, typename Func>
auto operator>>=(const Result<T> &r, Func f) -> decltype(f(std::get<T>(r))) {
  if (std::holds_alternative<T>(r)) {
    return f(std::get<T>(r));
  }
  return std::get<Error>(r);
}

// ── Currying Utility ──
// Allows writing auto add = curry([](int a, int b) { return a + b; });
// add(1)(2) -> 3

template <typename Function> struct Curried {
  Function f;

  template <typename... Args> auto operator()(Args &&...args) const {
    return f(std::forward<Args>(args)...);
  }
};

template <typename Function> auto curry(Function f) {
  return Curried<Function>{f};
}

// ── Pipe Operator (|>) ──
// Allows x |> f |> g instead of g(f(x))
template <typename T, typename Func>
    auto operator| > (T && val, Func &&f) -> decltype(f(std::forward<T>(val))) {
  return f(std::forward<T>(val));
}

} // namespace Core
} // namespace ForbocAI
