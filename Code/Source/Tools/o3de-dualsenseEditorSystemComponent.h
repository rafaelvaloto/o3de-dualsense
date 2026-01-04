
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <Clients/o3de-dualsenseSystemComponent.h>

namespace o3de_dualsense
{
    /// System component for o3de_dualsense editor
    class o3de_dualsenseEditorSystemComponent : public o3de_dualsenseSystemComponent,
                                                protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = o3de_dualsenseSystemComponent;

    public:
        AZ_COMPONENT_DECL(o3de_dualsenseEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        o3de_dualsenseEditorSystemComponent();
        ~o3de_dualsenseEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace o3de_dualsense
