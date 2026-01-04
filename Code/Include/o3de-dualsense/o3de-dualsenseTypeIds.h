
#pragma once

namespace o3de_dualsense
{
    // System Component TypeIds
    inline constexpr const char* o3de_dualsenseSystemComponentTypeId = "{AC4704B9-BF42-4C77-B893-5B3C246FA133}";
    inline constexpr const char* o3de_dualsenseEditorSystemComponentTypeId = "{CF1F4DAE-EED7-4F32-B148-0ADCFEB4C9C4}";

    // Module derived classes TypeIds
    inline constexpr const char* o3de_dualsenseModuleInterfaceTypeId = "{96090FC5-94FA-4C6F-AEF0-BA5095A66543}";
    inline constexpr const char* o3de_dualsenseModuleTypeId = "{720BDF44-9FB5-4EAB-B5C2-706CDC25147E}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* o3de_dualsenseEditorModuleTypeId = o3de_dualsenseModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* o3de_dualsenseRequestsTypeId = "{061C715E-105E-4D7A-838E-83067E596381}";
    inline constexpr const char* o3de_dualsenseDeviceId = "{E3391D84-18C6-4328-B02D-6421D846065E}";
} // namespace o3de_dualsense
