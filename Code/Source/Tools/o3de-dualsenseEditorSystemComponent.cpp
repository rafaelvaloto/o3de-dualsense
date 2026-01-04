
#include <AzCore/Serialization/SerializeContext.h>
#include "o3de-dualsenseEditorSystemComponent.h"

#include <o3de-dualsense/o3de-dualsenseTypeIds.h>

namespace o3de_dualsense
{
    AZ_COMPONENT_IMPL(o3de_dualsenseEditorSystemComponent, "o3de_dualsenseEditorSystemComponent",
        o3de_dualsenseEditorSystemComponentTypeId, BaseSystemComponent);

    void o3de_dualsenseEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<o3de_dualsenseEditorSystemComponent, o3de_dualsenseSystemComponent>()
                ->Version(0);
        }
    }

    o3de_dualsenseEditorSystemComponent::o3de_dualsenseEditorSystemComponent() = default;

    o3de_dualsenseEditorSystemComponent::~o3de_dualsenseEditorSystemComponent() = default;

    void o3de_dualsenseEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("o3de_dualsenseEditorService"));
    }

    void o3de_dualsenseEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("o3de_dualsenseEditorService"));
    }

    void o3de_dualsenseEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void o3de_dualsenseEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void o3de_dualsenseEditorSystemComponent::Activate()
    {
        o3de_dualsenseSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void o3de_dualsenseEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        o3de_dualsenseSystemComponent::Deactivate();
    }

} // namespace o3de_dualsense
