
set(FILES
        # --- Seus arquivos da Gem ---
        Source/o3de-dualsenseModuleInterface.cpp
        Source/o3de-dualsenseModuleInterface.h
        Source/Clients/o3de-dualsenseSystemComponent.cpp
        Source/Clients/o3de-dualsenseSystemComponent.h
        Source/Devices/DualSenseInputDevice.cpp
        Source/Devices/DualSenseInputDevice.h
        Source/Platforms/Windows/DualSenseWindowsPolicy.h
        Source/Platforms/Windows/DualSenseWindowsDeviceInfo.h
        Source/Adapters/DeviceRegistry.h

        # --- Arquivos do GamepadCore (OBRIGATÓRIOS) ---
        # Core
        Source/ThirdParty/GamepadCore/Source/Private/GCore/Interfaces/IPlatformHardwareInfo.cpp

        # Implementações Base
        Source/ThirdParty/GamepadCore/Source/Private/GImplementations/Libraries/Base/SonyGamepadAbstract.cpp

        # Implementações Específicas
        Source/ThirdParty/GamepadCore/Source/Private/GImplementations/Libraries/DualSense/DualSenseLibrary.cpp
        Source/ThirdParty/GamepadCore/Source/Private/GImplementations/Libraries/DualShock/DualShockLibrary.cpp

        # Utilitários
        Source/ThirdParty/GamepadCore/Source/Private/GImplementations/Utils/GamepadOutput.cpp

        # Áudio (Aquele que deu erro antes)
        Source/ThirdParty/GamepadCore/Source/Private/GImplementations/Utils/MiniAudio/miniaudio_impl.cpp
)
