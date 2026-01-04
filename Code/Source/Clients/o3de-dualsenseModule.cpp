
#include <o3de-dualsense/o3de-dualsenseTypeIds.h>
#include <o3de-dualsenseModuleInterface.h>
#include "o3de-dualsenseSystemComponent.h"

namespace o3de_dualsense
{
    class o3de_dualsenseModule
        : public o3de_dualsenseModuleInterface
    {
    public:
        AZ_RTTI(o3de_dualsenseModule, o3de_dualsenseModuleTypeId, o3de_dualsenseModuleInterface);
        AZ_CLASS_ALLOCATOR(o3de_dualsenseModule, AZ::SystemAllocator);
    };
}// namespace o3de_dualsense

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), o3de_dualsense::o3de_dualsenseModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_o3de_dualsense, o3de_dualsense::o3de_dualsenseModule)
#endif
