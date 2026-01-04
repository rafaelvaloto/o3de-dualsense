
#include "o3de-dualsenseModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <o3de-dualsense/o3de-dualsenseTypeIds.h>
#include <Clients/o3de-dualsenseSystemComponent.h>

namespace o3de_dualsense
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(o3de_dualsenseModuleInterface,
        "o3de_dualsenseModuleInterface", o3de_dualsenseModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(o3de_dualsenseModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(o3de_dualsenseModuleInterface, AZ::SystemAllocator);

    o3de_dualsenseModuleInterface::o3de_dualsenseModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            o3de_dualsenseSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList o3de_dualsenseModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<o3de_dualsenseSystemComponent>(),
        };
    }
} // namespace o3de_dualsense
