
#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include "Adapters/DeviceRegistry.h"
#include <o3de-dualsense/o3de-dualsenseBus.h>
#include "Platforms/Windows/DualSenseWindowsPolicy.h"
#include "Platforms/Windows/DualSenseWindowsDeviceInfo.h"
#include "Adapters/DeviceRegistry.h"

namespace o3de_dualsense
{
    class o3de_dualsenseSystemComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(o3de_dualsenseSystemComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        o3de_dualsenseSystemComponent();
        ~o3de_dualsenseSystemComponent() override;

        DeviceRegistry m_deviceRegistry;

        void Init() override {}
        void Activate() override;
        void Deactivate() override;

    protected:
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;


    };

} // namespace o3de_dualsense
