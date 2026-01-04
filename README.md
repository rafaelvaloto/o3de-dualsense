<p align="center">
  <img src="preview.png" alt="o3de-dualsense" width="200"/>
</p>

<h1 align="center">🎮 o3de-dualsense</h1>

<p align="center">
  <strong>Sony DualSense Controller Integration for O3DE</strong><br>
  <em>A prototype gem showcasing GamepadCore integration with the O3DE Input System</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/O3DE-Gem-blue?style=flat-square" alt="O3DE Gem"/>
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-green?style=flat-square" alt="Platform"/>
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=flat-square" alt="License"/>
  <img src="https://img.shields.io/badge/Status-Prototype-orange?style=flat-square" alt="Status"/>
</p>

---

## 📖 Overview

**o3de-dualsense** is a prototype gem that demonstrates how to integrate Sony DualSense (PS5) controllers into O3DE using [GamepadCore](https://github.com/user/gamepad-core). This project serves as a **starting point** and **reference implementation** for developers looking to add custom gamepad support to their O3DE projects.

> ⚠️ **Note:** This is a prototype for validating GamepadCore use cases. It provides a solid foundation but is not a production-ready implementation.

---

## ✨ Features

| Feature | Status |
|---------|--------|
| 🔌 Plug & Play device detection | ✅ |
| 🕹️ Face buttons (Cross, Circle, Square, Triangle) | ✅ |
| 🎯 Analog triggers (L2/R2) | ✅ |
| 🕹️ Analog sticks (Left/Right) | ✅ |
| 💡 Lightbar color control | ✅ |
| 📳 Haptic vibration | ✅ |
| 🔫 Adaptive triggers | ✅ |
| 🖥️ Windows platform support | ✅ |
| 🐧 Linux platform support | 🚧 Planned |

---

## 🏗️ Architecture

```
o3de-dualsense/
├── Code/
│   ├── Include/                    # Public headers
│   ├── Source/
│   │   ├── Adapters/               # GamepadCore ↔ O3DE bridge
│   │   │   └── DeviceRegistry.h    # Device management & EBus integration
│   │   ├── Clients/                # System component implementation
│   │   ├── Devices/                # O3DE InputDevice implementation
│   │   │   └── DualSenseInputDevice.cpp
│   │   ├── Platforms/
│   │   │   └── Windows/            # Platform-specific policies
│   │   └── ThirdParty/
│   │       └── GamepadCore/        # GamepadCore library
│   └── Platform/                   # Platform build configs
└── Registry/                       # Asset processor settings
```

---

## 🔧 How It Works

### The Input Pipeline

The gem creates a bridge between **GamepadCore** (low-level controller communication) and **O3DE's Input System** (high-level game input).

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  DualSense  │────▶│ GamepadCore │────▶│  Adapter    │────▶│ O3DE Input  │
│  Hardware   │     │  Library    │     │  Layer      │     │  System     │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

### Processing Raw Input

The core of the input translation happens in `DualSenseInputDevice::ProcessRawInput()`:

```cpp
void DualSenseInputDevice::ProcessRawInput(FDeviceContext* Context) {
    if (!Context) return;

    const auto& state = Context->GetInputState();

    // Face Buttons → O3DE Digital Channels
    if (buttonCross) buttonCross->SimulateRawInput(state.bCross);
    if (buttonCircle) buttonCircle->SimulateRawInput(state.bCircle);
    if (buttonSquare) buttonSquare->SimulateRawInput(state.bSquare);
    if (buttonTriangle) buttonTriangle->SimulateRawInput(state.bTriangle);

    // Analog Triggers → O3DE Analog Channels
    if (L2) L2->SimulateRawInput(state.LeftTriggerAnalog);
    if (R2) R2->SimulateRawInput(state.RightTriggerAnalog);

    // Analog Sticks → O3DE Axis2D Channels
    AZ::Vector2 leftStick(state.LeftAnalog.X, state.LeftAnalog.Y);
    if (AnalogLeft) AnalogLeft->SimulateRawInput2D(leftStick.GetX(), leftStick.GetY());

    AZ::Vector2 rightStick(state.RightAnalog.X, state.RightAnalog.Y);
    if (AnalogRight) AnalogRight->SimulateRawInput2D(rightStick.GetX(), rightStick.GetY());
}
```

**Key Points:**
- `FDeviceContext` provides the raw controller state from GamepadCore
- `SimulateRawInput()` feeds values into O3DE's input channel system
- Buttons map to standard gamepad buttons (A/B/X/Y)
- Triggers provide analog 0.0-1.0 values
- Sticks provide 2D axis values

### Device Registry Pattern

The `DeviceRegistry` manages controller lifecycle using a **policy-based design**:

```cpp
struct DeviceRegistryPolicy {
    using EngineIdType = AZ::u32;
    std::unordered_map<EngineIdType, std::unique_ptr<DualSenseInputDevice>> CustomDevicesMap;

    void DispatchNewGamepad(EngineIdType GamepadId) {
        InputDeviceId deviceName(AZStd::string_view("dualsense"), GamepadId);
        auto DualInputDevice = std::make_unique<DualSenseInputDevice>(deviceName);
        CustomDevicesMap[GamepadId] = std::move(DualInputDevice);
        CustomDevicesMap[GamepadId]->SetConnected(true);
    }

    void DisconnectDevice(EngineIdType GamepadId) {
        CustomDevicesMap.erase(GamepadId);
    }
};
```

This pattern allows:
- 🔄 Automatic device detection and registration
- 🎮 Multiple controller support
- 🔌 Hot-plug/unplug handling
- 📡 O3DE EBus integration

### Platform Abstraction

Platform-specific code is isolated using policy classes:

```cpp
struct WindowsHardwarePolicy {
    void Read(FDeviceContext* Context) {
        WindowsDeviceInfo::Read(Context);
    }

    void Write(FDeviceContext* Context) {
        WindowsDeviceInfo::Write(Context);
    }

    void Detect(std::vector<FDeviceContext>& Devices) {
        WindowsDeviceInfo::Detect(Devices);
    }

    void ProcessAudioHaptic(FDeviceContext* Context) {
        WindowsDeviceInfo::ProcessAudioHapitc(Context);
    }
};
```

---

## 🚀 Quick Start

### 1. Add the Gem to Your Project

Copy the gem to your O3DE project's `Gems/` directory or register it globally.

### 2. Enable the Gem

Add to your project's `project.json`:
```json
{
    "gem_dependencies": [
        "o3de-dualsense"
    ]
}
```

### 3. Build Your Project

```bash
cmake --build build/windows --target YourProject.GameLauncher --config profile
```

### 4. Connect Your DualSense

Plug in your DualSense controller via USB. The gem automatically detects and registers it.

---

## 🎮 Using the Input

Once connected, DualSense inputs are available through O3DE's standard input system:

| DualSense Button | O3DE Input Channel |
|-----------------|-------------------|
| ✕ Cross | `gamepad_button_a` |
| ○ Circle | `gamepad_button_b` |
| □ Square | `gamepad_button_x` |
| △ Triangle | `gamepad_button_y` |
| L2 | `gamepad_trigger_l2` |
| R2 | `gamepad_trigger_r2` |
| Left Stick | `gamepad_stick_l` |
| Right Stick | `gamepad_stick_r` |

---

## 💡 DualSense-Specific Features

### Lightbar Control
```cpp
gamepad->SetLightbar({255, 0, 0}); // Set to red
```

### Haptic Vibration
```cpp
gamepad->SetVibration(10, 30); // Left motor: 10, Right motor: 30
```

### Adaptive Triggers
```cpp
IGamepadTrigger* Trigger = gamepad->GetIGamepadTrigger();
// Bow effect on L2
Trigger->SetBow22(0xf8, 0x3f, EDSGamepadHand::Left);
// Machine gun effect on R2
Trigger->SetMachine27(0x80, 0x02, 0x03, 0x0f, 0x19, 0x02, EDSGamepadHand::Right);
```

---

## 📁 Project Structure

| Directory | Purpose |
|-----------|---------|
| `Code/Include/` | Public API headers |
| `Code/Source/Adapters/` | GamepadCore ↔ O3DE bridge layer |
| `Code/Source/Clients/` | System component implementation |
| `Code/Source/Devices/` | O3DE InputDevice wrapper |
| `Code/Source/Platforms/` | Platform-specific implementations |
| `Code/Source/ThirdParty/` | GamepadCore library |
| `Code/Platform/` | CMake platform configurations |

---

## 🛠️ Extending This Prototype

This gem provides a solid foundation. Here's how to extend it:

### Add More Buttons
1. Add new `InputChannel*` members in `DualSenseInputDevice.h`
2. Create and register them in the constructor
3. Process them in `ProcessRawInput()`

### Add Linux Support
1. Create `Source/Platforms/Linux/DualSenseLinuxPolicy.h`
2. Implement the policy interface using Linux input subsystem
3. Register the policy in the system component

### Add Bluetooth Support
1. Extend `WindowsDeviceInfo` to detect Bluetooth connections
2. Handle different report formats for USB vs Bluetooth

---

## 📋 Requirements

- **O3DE** 23.10 or later
- **GamepadCore** library (included in `ThirdParty/`)
- **Windows 10/11** (for Windows builds)
- **DualSense Controller** (PS5 controller)

---

## 🤝 Contributing

Contributions are welcome! This is a prototype meant to evolve.

1. Fork the repository
2. Create your feature branch
3. Implement your changes
4. Submit a pull request

---

## 📄 License

This project is licensed under the **MIT License** - see the gem.json for details.

---

## ⚖️ Disclaimer and Trademarks

This software is an independent and unofficial project. It is **not** affiliated, associated, authorized, endorsed by, or in any way officially connected with Sony Interactive Entertainment Inc., Microsoft Corporation, Apple Inc., Epic Games, Unity Technologies, the Godot Engine project, or the Open 3D Foundation.

**Trademarks belong to their respective owners:**

* **Sony:** "PlayStation", "PlayStation Family Mark", "PS5 logo", "PS5", "DualSense", and "DualShock" are registered trademarks or trademarks of Sony Interactive Entertainment Inc. "SONY" is a registered trademark of Sony Corporation.
* **Microsoft:** "Windows" and "Xbox" are registered trademarks of Microsoft Corporation.
* **Apple:** "Mac" and "macOS" are registered trademarks of Apple Inc.
* **Linux:** "Linux" is the registered trademark of Linus Torvalds in the U.S. and other countries.
* **Epic Games:** "Unreal" and "Unreal Engine" are trademarks or registered trademarks of Epic Games, Inc. in the United States of America and elsewhere.
* **Unity:** "Unity", Unity logos, and other Unity trademarks are trademarks or registered trademarks of Unity Technologies or its affiliates in the U.S. and elsewhere.
* **Godot:** "Godot" and the Godot logo are trademarks of the Godot Engine project.
* **O3DE:** "O3DE" and the O3DE logo are trademarks of the Open 3D Foundation.
