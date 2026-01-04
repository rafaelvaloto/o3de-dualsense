//
// Created by rafae on 1/3/2026.
//

#include "DualSenseInputDevice.h"

#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>
namespace o3de_dualsense
{
    using namespace AzFramework;

    DualSenseInputDevice::DualSenseInputDevice(const InputDeviceId& inputDeviceId) : InputDevice(inputDeviceId)
    {
        // Cross (X) -> button A (South)
        buttonCross = aznew InputChannelDigital(InputDeviceGamepad::Button::A, *this);

        // Circle (O) -> button B (East)
        buttonCircle = aznew InputChannelDigital(InputDeviceGamepad::Button::B, *this);

        // Square ([]) -> button X (West)
        buttonSquare = aznew InputChannelDigital(InputDeviceGamepad::Button::X, *this);

        // Triangle (/\) -> button Y (North)
        buttonTriangle = aznew InputChannelDigital(InputDeviceGamepad::Button::Y, *this);

        // L2 e R2 (Triggers)
        L2 = aznew InputChannelAnalog(InputDeviceGamepad::Trigger::L2, *this);
        R2 = aznew InputChannelAnalog(InputDeviceGamepad::Trigger::R2, *this);

        // Sticks (L3/R3)
        AnalogLeft = aznew InputChannelAxis2D(InputDeviceGamepad::ThumbStickAxis2D::L, *this);
        AnalogRight = aznew InputChannelAxis2D(InputDeviceGamepad::ThumbStickAxis2D::R, *this);

        m_allChannelsById[buttonCross->GetInputChannelId()] = buttonCross;
        m_allChannelsById[buttonCircle->GetInputChannelId()] = buttonCircle;
        m_allChannelsById[buttonSquare->GetInputChannelId()] = buttonSquare;
        m_allChannelsById[buttonTriangle->GetInputChannelId()] = buttonTriangle;

        m_allChannelsById[L2->GetInputChannelId()] = L2;
        m_allChannelsById[R2->GetInputChannelId()] = R2;
        m_allChannelsById[AnalogLeft->GetInputChannelId()] = AnalogLeft;
        m_allChannelsById[AnalogRight->GetInputChannelId()] = AnalogRight;
    }

    DualSenseInputDevice::~DualSenseInputDevice()
    {
        for (const auto& pair : m_allChannelsById)
        {
            delete pair.second;
        }
        m_allChannelsById.clear();
    }

    const InputDeviceRequests::InputChannelByIdMap& DualSenseInputDevice::GetInputChannelsById() const
    {
        return m_allChannelsById;
    }

    const InputDeviceId& DualSenseInputDevice::GetInputDeviceId() const
    {
        return m_deviceId;
    }

    bool DualSenseInputDevice::IsSupported() const
    {
        return true;
    }

    bool DualSenseInputDevice::IsConnected() const
    {
        return m_connected;
    }

    void DualSenseInputDevice::TickInputDevice() {}

    void DualSenseInputDevice::SetConnected(bool connected)
    {
        if (m_connected != connected)
        {
            m_connected = connected;
            BroadcastInputDeviceConnectedEvent();
            AZ_Printf("DualSense", "BroadcastInputDeviceConnectedEvent");
        }
    }

    void DualSenseInputDevice::ProcessRawInput(FDeviceContext* Context)
    {
        if (!Context)
            return;

        const auto& state = Context->GetInputState();

        if (buttonCross)
            buttonCross->SimulateRawInput(state.bCross);
        if (buttonCircle)
            buttonCircle->SimulateRawInput(state.bCircle);
        if (buttonSquare)
            buttonSquare->SimulateRawInput(state.bSquare);
        if (buttonTriangle)
            buttonTriangle->SimulateRawInput(state.bTriangle);

        if (L2)
            L2->SimulateRawInput(state.LeftTriggerAnalog);
        if (R2)
            R2->SimulateRawInput(state.RightTriggerAnalog);

        AZ::Vector2 leftStick(state.LeftAnalog.X, state.LeftAnalog.Y);
        if (AnalogLeft)
            AnalogLeft->SimulateRawInput2D(leftStick.GetX(), leftStick.GetY());

        AZ::Vector2 rightStick(state.RightAnalog.X, state.RightAnalog.Y);
        if (AnalogRight)
            AnalogRight->SimulateRawInput2D(rightStick.GetX(), rightStick.GetY());

        if (buttonCross->GetValue() || buttonCircle->GetValue() || buttonSquare->GetValue() ||
            buttonTriangle->GetValue())
        {
            AZ_Printf("DualSense", "buttonCross: %f, buttonCircle: %f buttonSquare: %f buttonTriangle: %f",
                      buttonCross->GetValue(), buttonCircle->GetValue(), buttonSquare->GetValue(),
                      buttonTriangle->GetValue());
        }

        if (L2->GetValue() > 0.0f || R2->GetValue() > 0.0f)
        {
            AZ_Printf("DualSense", "L2: %f, R2: %f", L2->GetValue(), R2->GetValue());
        }
    }
} // namespace o3de_dualsense