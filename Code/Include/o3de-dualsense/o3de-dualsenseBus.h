
#pragma once

#include <o3de-dualsense/o3de-dualsenseTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace o3de_dualsense
{
    class o3de_dualsenseRequests
    {
    public:
        AZ_RTTI(o3de_dualsenseRequests, o3de_dualsenseRequestsTypeId);
        virtual ~o3de_dualsenseRequests() = default;
        // Put your public methods here
    };

    class o3de_dualsenseBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using o3de_dualsenseRequestBus = AZ::EBus<o3de_dualsenseRequests, o3de_dualsenseBusTraits>;
    using o3de_dualsenseInterface = AZ::Interface<o3de_dualsenseRequests>;

} // namespace o3de_dualsense
