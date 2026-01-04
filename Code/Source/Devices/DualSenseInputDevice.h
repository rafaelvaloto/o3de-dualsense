#pragma once

#include <AzFramework/Input/Devices/InputDevice.h>
#include <GCore/Interfaces/ISonyGamepad.h>
#include <AzFramework/Input/Channels/InputChannelDigital.h>
#include <AzFramework/Input/Channels/InputChannelAxis2D.h>
#include <AzFramework/Input/Channels/InputChannelAnalog.h>

namespace AzFramework {
    class InputChannelDigital;
}

namespace o3de_dualsense
{
    using namespace AzFramework;
    class DualSenseInputDevice : public InputDevice
    {
    public:
        AZ_CLASS_ALLOCATOR(DualSenseInputDevice, AZ::SystemAllocator);
        AZ_RTTI(InputDevice, o3de_dualsenseDeviceId);

        explicit DualSenseInputDevice(const InputDeviceId& inputDeviceId);
        ~DualSenseInputDevice() override;

        const InputDeviceId& GetInputDeviceId() const override;
        const InputChannelByIdMap& GetInputChannelsById() const override;

        void TickInputDevice() override;
        bool IsSupported() const override;
        bool IsConnected() const override;

        void SetConnected(bool connected);
        void ProcessRawInput(FDeviceContext* Context);

    protected:
        bool m_connected = false;
        InputDeviceId m_deviceId;
        InputChannelByIdMap m_allChannelsById;


        InputChannelDigital* buttonCross;
        InputChannelDigital* buttonSquare;
        InputChannelDigital* buttonTriangle;
        InputChannelDigital* buttonCircle;
        InputChannelAxis2D* AnalogLeft;
        InputChannelAxis2D* AnalogRight;
        InputChannelAnalog* L2;
        InputChannelAnalog* R2;
    };
}
