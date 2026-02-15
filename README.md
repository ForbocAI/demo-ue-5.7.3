
# ForbocAI SDK Demo Project (UE 5.7)

This project demonstrates the integration of the **ForbocAI SDK** with
Unreal Engine 5.7, showcasing **Neuro-Symbolic Agent** creation and
interaction via both C++ and Blueprints.

The SDK follows **strict Functional C++11** — pure data structs, factory
functions, and free (pure) functions only. The demo's `ASDKTestActor` is
a thin UE boundary layer that delegates all logic to SDK-side factories
and operations.

---

## Prerequisites

| Tool | Version |
|------|---------|
| Unreal Engine | 5.7 |
| Visual Studio | 2022+ (Windows) |
| Xcode | 15+ (macOS) |

## Setup

1. **Generate Project Files**
   - Right-click `DemoProject.uproject`
   - Select *Generate Visual Studio Project Files* (Windows) or
     *Generate Xcode Project* (macOS)

2. **Open Solution**
   - Open the generated `.sln` or `.xcworkspace`

3. **Build**
   - Build for **Development Editor** configuration

4. **Launch**
   - Launch from your IDE or double-click `DemoProject.uproject`

---

## Architecture

The demo follows the SDK's strict Functional Programming rules:

| Rule | How the Demo Applies It |
|------|------------------------|
| Data = structs only | `FAgent`, `FAgentState`, `FAgentResponse` are plain data |
| Behaviour = free functions | `AgentFactory::Create`, `AgentOps::Process`, `AgentOps::WithState` |
| No mutation | `FAgent` has `const` members; updates return new values |
| Factory functions | `AgentFactory::Create(Config)` replaces constructors |
| `TypeFactory` for USTRUCTs | `TypeFactory::AgentState(Mood)` creates UE-reflected data |
| Immutable references | `TSharedPtr<const FAgent>` rebinds pointer, never mutates data |

The `ASDKTestActor` class itself is a **UE framework obligation** (actors
require `UCLASS`, a constructor, and virtual overrides). It does not
represent an FP violation — it is the *boundary* between UE's OOP
framework and the SDK's functional core.

---

## C++ Integration

`SDKTestActor.h` / `SDKTestActor.cpp` demonstrate the three core
operations:

### 1. Create Agent (Factory)

```cpp
FAgentConfig Config;
Config.Persona = TEXT("Cyber-Merchant");
Config.ApiUrl  = TEXT("https://api.forboc.ai");

// Factory function returns immutable FAgent
CurrentAgent = MakeShared<const FAgent>(AgentFactory::Create(Config));
```

### 2. Process Input (Free Function)

```cpp
const FAgentResponse Response =
    AgentOps::Process(*CurrentAgent, InputText, {});

// Response.Dialogue contains the agent's reply
OnAgentResponse(Response.Dialogue);
```

### 3. Update State (Immutable Transformation)

```cpp
const FAgentState NewState = TypeFactory::AgentState(TEXT("Alert"));

// Returns a NEW agent — original stays untouched
CurrentAgent =
    MakeShared<const FAgent>(AgentOps::WithState(*CurrentAgent, NewState));
```

---

## Blueprint Usage

The project includes `ASDKTestActor`, designed to be extended in
Blueprints.

1. **Create Blueprint**
   - Content Browser → New Blueprint Class → parent: `SDKTestActor`
   - Name it `BP_SDKTestAgent`

2. **Configure**
   - Open `BP_SDKTestAgent` → Details / Class Defaults
   - **Persona**: e.g. `"Cyber-Merchant"`
   - **Api Url**: `https://api.forboc.ai`

3. **Implement Events**
   - Event Graph → implement **Event On Agent Response**
   - Wire to `Print String` to see the response

4. **Test**
   - `Initialize Agent` fires on `BeginPlay` automatically
   - Call `Process Input` on a key press or UI event:
     `"Hello, who are you?"`
   - Call `Update Agent State` to change mood/context

---

## Features Tested

| Feature | How |
|---------|-----|
| Plugin Integration | `ForbocAI_SDK` plugin loads at runtime |
| Agent Factory | `AgentFactory::Create` produces an immutable `FAgent` |
| Functional Ops | `AgentOps::Process` handles input → response |
| State Management | `AgentOps::WithState` returns new agent, never mutates |
| Blueprint Interop | `BlueprintCallable` / `BlueprintImplementableEvent` |

---

## Project Structure

```
demo-ue-5.7.3/
├── DemoProject.uproject
├── Source/
│   ├── DemoProject.Target.cs          # Game target (V6, UE 5.7)
│   ├── DemoProjectEditor.Target.cs    # Editor target (V6, UE 5.7)
│   └── DemoProject/
│       ├── DemoProject.Build.cs
│       ├── DemoProject.h / .cpp       # Module boilerplate
│       └── SDKTestActor.h / .cpp      # Demo actor
├── Plugins/
│   └── ForbocAI_SDK/                  # SDK plugin (copied)
│       ├── ForbocAI_SDK.uplugin
│       └── Source/ForbocAI_SDK/...
└── README.md
```

> **Note**: `Binaries/` and `Intermediate/` are build artifacts, excluded
> by `.gitignore`. Consider replacing the copied plugin with a
> **git submodule** pointing to `sdk-ue-5.7.3` to keep a single source
> of truth.

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| "Plugin 'ForbocAI_SDK' failed to load" | Ensure `Plugins/ForbocAI_SDK/` has compiled binaries |
| Linker errors after C++ changes | Regenerate project files from `.uproject` |
| `FAgent` assignment errors | Must use `TSharedPtr<const FAgent>` — `FAgent` has `const` members |
| Missing `Response.Content` | Field is `Response.Dialogue` (not `.Content`) |
| Warnings about `BuildSettingsVersion` | Both Target.cs files should use `V6` + `Unreal5_7` |

---

## References

- [`sdk-ue-5.7.3/README.md`](../sdk-ue-5.7.3/README.md) — Full SDK documentation
- [`sdk-ue-5.7.3/C++11-FP-GUIDE.md`](../sdk-ue-5.7.3/C++11-FP-GUIDE.md) — Functional architecture guide
- [`sdk-ue-5.7.3/style-guide.md`](../sdk-ue-5.7.3/style-guide.md) — Aesthetic protocol

---
(c) 2026 ForbocAI
