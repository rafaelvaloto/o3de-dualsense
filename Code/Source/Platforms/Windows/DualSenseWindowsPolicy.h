#pragma once

#include "DualSenseWindowsDeviceInfo.h"

#include <GCore/Templates/TGenericHardwareInfo.h>

namespace o3de_dualsense
{
    // Forward declaration
    struct WindowsHardwarePolicy;
    using WindowsHardware = TGenericHardwareInfo<WindowsHardwarePolicy>;

    struct WindowsHardwarePolicy
    {
    public:
        void Read(FDeviceContext* Context) { WindowsDeviceInfo::Read(Context); }

        void Write(FDeviceContext* Context) { WindowsDeviceInfo::Write(Context); }

        void Detect(std::vector<FDeviceContext>& Devices) { WindowsDeviceInfo::Detect(Devices); }

        bool CreateHandle(FDeviceContext* Context) { return WindowsDeviceInfo::CreateHandle(Context); }

        void InvalidateHandle(FDeviceContext* Context) { WindowsDeviceInfo::InvalidateHandle(Context); }

        void ProcessAudioHaptic(FDeviceContext* Context) { WindowsDeviceInfo::ProcessAudioHapitc(Context); }

        void InitializeAudioDevice(FDeviceContext* /*Context*/)
        {
            // WindowsDeviceInfo::InitializeAudioDevice(Context);
        }
    };
} // namespace o3de_dualsense
