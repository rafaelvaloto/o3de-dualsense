
#include "o3de-dualsenseEditorSystemComponent.h"

#include <o3de-dualsense/o3de-dualsenseTypeIds.h>
#include <o3de-dualsenseModuleInterface.h>

namespace o3de_dualsense
{
    class o3de_dualsenseEditorModule : public o3de_dualsenseModuleInterface
    {
    public:
        AZ_RTTI(o3de_dualsenseEditorModule, o3de_dualsenseEditorModuleTypeId, o3de_dualsenseModuleInterface);
        AZ_CLASS_ALLOCATOR(o3de_dualsenseEditorModule, AZ::SystemAllocator);

        o3de_dualsenseEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext,
            // BehaviorContext and EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                                                          o3de_dualsenseEditorSystemComponent::CreateDescriptor(),
                                                      });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<o3de_dualsenseEditorSystemComponent>(),
            };
        }
    };
} // namespace o3de_dualsense

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), o3de_dualsense::o3de_dualsenseEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_o3de_dualsense_Editor, o3de_dualsense::o3de_dualsenseEditorModule)
#endif
