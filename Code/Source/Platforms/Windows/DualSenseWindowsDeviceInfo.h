#pragma once
#ifdef _WIN32

#include "GCore/Types/Structs/Config/GamepadCalibration.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GImplementations/Utils/GamepadSensors.h"

#include <Windows.h>
#include <filesystem>
#include <hidsdi.h>
#include <initguid.h>
#include <setupapi.h>
#include <vector>

/**
 * @brief Enumerates the possible outcomes of a polling operation in HID device communication.
 *
 * This enum is used to represent the results of data polling when interacting with HID devices.
 * It provides status indicators for successful reads, lack of data in the current tick,
 * transient errors, or disconnected devices.
 */
enum class ETest_PollResult
{
    ReadOk,
    NoIoThisTick,
    TransientError,
    Disconnected
};

namespace o3de_dualsense
{
    class WindowsDeviceInfo
    {
    public:
        WindowsDeviceInfo() = default;

        ~WindowsDeviceInfo() = default;

        static void InitializeAudioDevice(FDeviceContext* Context)
        {
            if (!Context)
            {
                return;
            }

            // Initialize miniaudio context for device enumeration
            ma_context maContext;
            if (ma_context_init(nullptr, 0, nullptr, &maContext) != MA_SUCCESS)
            {
                return;
            }

            // Get playback devices
            ma_device_info* pPlaybackInfos;
            ma_uint32 playbackCount;
            ma_device_info* pCaptureInfos;
            ma_uint32 captureCount;

            if (ma_context_get_devices(&maContext, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) !=
                MA_SUCCESS)
            {
                ma_context_uninit(&maContext);
                return;
            }

            // Search for DualSense audio device
            ma_device_id* pFoundDeviceId = nullptr;
            ma_device_id foundDeviceId;

            for (ma_uint32 i = 0; i < playbackCount; i++)
            {
                std::string deviceName(pPlaybackInfos[i].name);

                // Check if device name contains DualSense identifiers
                // DualSense appears as "Wireless Controller" or "DualSense Wireless Controller"
                if (deviceName.find("DualSense") != std::string::npos ||
                    deviceName.find("Wireless Controller") != std::string::npos)
                {
                    // Verify it has 4 channels (stereo + haptic L/R)
                    // DualSense audio device typically has 4 channels for haptics
                    foundDeviceId = pPlaybackInfos[i].id;
                    pFoundDeviceId = &foundDeviceId;
                    break;
                }
            }

            // Initialize audio context with found device (or default if not found)
            Context->AudioContext = std::make_shared<FAudioDeviceContext>();

            if (pFoundDeviceId)
            {
                // Initialize with specific DualSense device
                // DualSense haptics use 4 channels at 48000 Hz
                Context->AudioContext->InitializeWithDeviceId(pFoundDeviceId, 48000, 4);
            }

            ma_context_uninit(&maContext);
        }

        static void ProcessAudioHapitc(FDeviceContext* Context)
        {
            if (!Context || !Context->Handle)
            {
                return;
            }

            if (Context->ConnectionType != EDSDeviceConnection::Bluetooth)
            {
                return;
            }

            unsigned long BytesWritten = 0;
            constexpr size_t BufferSize = 142;
            if (!WriteFile(Context->Handle, Context->BufferAudio, BufferSize, &BytesWritten, nullptr))
            {
                const unsigned long Error = GetLastError();
                if (Error != ERROR_IO_PENDING)
                {
                }
            }
        }

        /**
         * @brief Configures Bluetooth-specific features for a given HID device.
         *
         * This method is used to initialize and set up Bluetooth-related features on a HID device.
         * It communicates with the device using a feature report and updates the provided device context accordingly.
         *
         * @param Context A reference to the device context object that holds device-specific settings and state
         * information.
         * @return A boolean indicating whether the Bluetooth feature configuration was successful (true) or failed
         * (false).
         */
        static void ConfigureFeatures(FDeviceContext* Context)
        {
            unsigned char FeatureBuffer[41] = {0};
            std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

            FeatureBuffer[0] = 0x05;
            if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 41))
            {
                const unsigned long Error = GetLastError();
                if (Error != 0)
                {
                    AZ_Printf("DualSense", "Falha detectada no Windows. Código de erro: %lu", Error);
                }
                return;
            }

            FGamepadCalibration Calibration;
            using namespace FGamepadSensors;
            DualSenseCalibrationSensors(FeatureBuffer, Calibration);

            Context->Calibration = Calibration;
        }

        /**
         * @brief Reads data from the specified HID device context.
         *
         * This method performs the process of retrieving input data from an HID device, verifying the context's
         * validity, handling connection states, and managing input buffers. It also utilizes polling mechanisms to
         * detect and report disconnection events or communication failures during the read process.
         *
         * @param Context Pointer to the device context representing the HID device being read. Must not be null and
         *        should contain a valid device handle and input buffer. If the context is invalid or disconnected,
         *        the method will handle associated cleanup and reporting.
         */
        static void Read(FDeviceContext* Context)
        {
            if (!Context)
            {
                return;
            }

            if (Context->Handle == INVALID_PLATFORM_HANDLE)
            {
                return;
            }

            if (!Context->IsConnected)
            {
                return;
            }

            DWORD BytesRead = 0;
            if (Context->ConnectionType == EDSDeviceConnection::Bluetooth &&
                Context->DeviceType == EDSDeviceType::DualShock4)
            {
                constexpr size_t InputReportLength = 547;
                PollTick(Context->Handle, Context->BufferDS4, InputReportLength, BytesRead);
            }
            else
            {
                const size_t ReportLength = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : 64;
                PollTick(Context->Handle, Context->Buffer, static_cast<DWORD>(ReportLength), BytesRead);
            }
        }

        /**
         * @brief Writes data to the specified HID device context.
         *
         * This method handles sending data to an HID device using the provided device context.
         * It determines the appropriate output report size based on the device type and connection type,
         * ensures the device handle is valid, and writes the data. Handles errors such as failed writes
         * or invalid handles and logs necessary debugging information.
         *
         * @param Context Pointer to the device context containing relevant information such as the device
         *        handle, connection type, device type, and output buffer. Must not be null and must
         *        represent a valid device handle for a successful write operation.
         */
        static void Write(FDeviceContext* Context)
        {
            if (Context->Handle == INVALID_HANDLE_VALUE)
            {
                return;
            }

            size_t InReportLength = Context->DeviceType == EDSDeviceType::DualShock4 ? 32 : 74;
            size_t OutputReportLength = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : InReportLength;

            DWORD BytesWritten = 0;
            if (!WriteFile(Context->Handle, Context->BufferOutput, static_cast<DWORD>(OutputReportLength),
                           &BytesWritten, nullptr))
            {
            }
        }

        /**
         * @brief Detects available HID devices and updates the provided list of device contexts.
         *
         * This method performs an enumeration of all connected HID devices that are currently present and
         * accessible on the system. It retrieves their attributes, device paths, and initializes valid device
         * contexts for further interaction. Devices that cannot be accessed or initialized are skipped.
         *
         * @param Devices A reference to an array of FDeviceContext objects that will be updated to include
         *        the detected and initialized HID device contexts. Existing data in the array will be overwritten.
         */
        static void Detect(std::vector<FDeviceContext>& Devices)
        {
            GUID HidGuid;
            HidD_GetHidGuid(&HidGuid);

            const HDEVINFO DeviceInfoSet =
                SetupDiGetClassDevs(&HidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            if (DeviceInfoSet == INVALID_HANDLE_VALUE)
            {
                return;
            }

            SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
            DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

            std::unordered_map<int, std::string> DevicePaths;
            for (int DeviceIndex = 0;
                 SetupDiEnumDeviceInterfaces(DeviceInfoSet, nullptr, &HidGuid, DeviceIndex, &DeviceInterfaceData);
                 DeviceIndex++)
            {
                DWORD RequiredSize = 0;
                SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, nullptr, 0, &RequiredSize,
                                                nullptr);

                const auto DetailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(RequiredSize));
                if (!DetailDataBuffer)
                {
                    continue;
                }

                DetailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                if (SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, DetailDataBuffer, RequiredSize,
                                                    nullptr, nullptr))
                {

                    std::wstring MyStdString = std::filesystem::path(DetailDataBuffer->DevicePath).wstring();
                    const HANDLE TempDeviceHandle =
                        CreateFileW(MyStdString.data(), GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

                    if (TempDeviceHandle != INVALID_HANDLE_VALUE)
                    {
                        HIDD_ATTRIBUTES Attributes = {};
                        Attributes.Size = sizeof(HIDD_ATTRIBUTES);

                        if (HidD_GetAttributes(TempDeviceHandle, &Attributes))
                        {
                            if (Attributes.VendorID == 0x054C &&
                                (Attributes.ProductID == 0x0CE6 || Attributes.ProductID == 0x0DF2 ||
                                 Attributes.ProductID == 0x05C4 || Attributes.ProductID == 0x09CC))
                            {
                                FDeviceContext Context = {};
                                wchar_t DeviceProductString[260];
                                if (HidD_GetProductString(TempDeviceHandle, DeviceProductString, 260))
                                {
                                    if (DevicePaths.find(DeviceIndex) == DevicePaths.end())
                                    {
                                        std::string FinalString =
                                            std::filesystem::path(DetailDataBuffer->DevicePath).string();
                                        Context.Path = FinalString;
                                        DevicePaths[DeviceIndex] = FinalString;
                                        switch (Attributes.ProductID)
                                        {
                                        case 0x05C4:
                                        case 0x09CC:
                                            Context.DeviceType = EDSDeviceType::DualShock4;
                                            break;
                                        case 0x0DF2:
                                            Context.DeviceType = EDSDeviceType::DualSenseEdge;
                                            break;
                                        default:
                                            Context.DeviceType = EDSDeviceType::DualSense;
                                        }

                                        Context.IsConnected = true;
                                        Context.ConnectionType = EDSDeviceConnection::Usb;
                                        const std::string BtGuid = "{00001124-0000-1000-8000-00805f9b34fb}";
                                        if (FinalString.find(BtGuid) != std::string::npos ||
                                            FinalString.find("bth") != std::string::npos ||
                                            FinalString.find("BTHENUM") != std::string::npos)
                                        {
                                            Context.ConnectionType = EDSDeviceConnection::Bluetooth;
                                        }
                                    }
                                    Devices.push_back(Context);
                                }
                            }
                        }
                    }
                    CloseHandle(TempDeviceHandle);
                }
                free(DetailDataBuffer);
            }
            SetupDiDestroyDeviceInfoList(DeviceInfoSet);
        }

        /**
         * @brief Creates a handle for the specified device context.
         *
         * This method attempts to open a communication channel with the device specified in the provided
         * context by creating a handle. If successful, the handle is associated with the device context.
         *
         * @param Context A pointer to the device context containing the device path and other relevant information.
         * @return True if the handle creation is successful; otherwise, false.
         */
        static bool CreateHandle(FDeviceContext* Context)
        {
            std::string Source = Context->Path;
            std::wstring MyStdString = std::filesystem::path(Source).wstring();
            const HANDLE DeviceHandle =
                CreateFileW(MyStdString.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr, OPEN_EXISTING, 0, nullptr);

            if (DeviceHandle == INVALID_PLATFORM_HANDLE)
            {
                Context->Handle = DeviceHandle;
                return false;
            }

            Context->Handle = DeviceHandle;
            ConfigureFeatures(Context);
            return true;
        }

        /**
         * @brief Invalidates the handle of the specified HID device context and updates its connection status.
         *
         * This method ensures that the handle associated with the provided device context is properly invalidated.
         * If the handle is valid, it will be closed and set to INVALID_HANDLE_VALUE. The connection status of the
         * device context will also be updated to indicate that the device is no longer connected.
         *
         * @param Context Pointer to the device context representing the HID device whose handle is to be invalidated.
         *        If the provided context is null, the method will return without performing any operations.
         */
        static void InvalidateHandle(FDeviceContext* Context)
        {
            if (!Context)
            {
                return;
            }

            if (Context->Handle != INVALID_PLATFORM_HANDLE)
            {
                CloseHandle(Context->Handle);
                Context->Handle = INVALID_PLATFORM_HANDLE;
                Context->IsConnected = false;
                Context->Path.clear();

                ZeroMemory(Context->BufferOutput, sizeof(Context->BufferOutput));
                ZeroMemory(Context->BufferAudio, sizeof(Context->BufferAudio));
                ZeroMemory(Context->Buffer, sizeof(Context->Buffer));
                ZeroMemory(Context->BufferDS4, sizeof(Context->BufferDS4));
            }
        }

        /**
         * @brief Sends a single ping operation to check the state of the specified handle.
         *
         * This method determines whether the provided handle is in a valid state and retrieves
         * additional error information if the ping operation fails.
         *
         * @param Handle The handle to be checked.
         * @param OutLastError Pointer to an integer that receives the error code if the operation fails.
         *                     On successful execution, this will be set to ERROR_SUCCESS.
         * @return True if the ping operation succeeds; otherwise, false.
         */
        static bool PingOnce(HANDLE Handle, std::int32_t* OutLastError = nullptr)
        {
            FILE_STANDARD_INFO Info{};
            if (!GetFileInformationByHandleEx(Handle, FileStandardInfo, &Info, sizeof(Info)))
            {
                if (OutLastError)
                {
                    *OutLastError = GetLastError();
                }
                return false;
            }
            if (OutLastError)
            {
                *OutLastError = ERROR_SUCCESS;
            }
            return true;
        }

        /**
         * @brief Polls and processes a single tick for a HID device, performing ping and read operations.
         *
         * This method checks the device's state, performs a ping operation if necessary, and reads data from the
         * device. It updates the polling state accordingly and determines the result of this polling iteration.
         *
         * @param Handle A handle to the HID device being polled.
         * @param Buffer A pointer to a buffer where the method writes the data read from the device.
         * @param Length The maximum number of bytes that can be read into the buffer.
         * @param OutBytesRead A reference to a variable where the number of bytes successfully read will be stored.
         * @return An enumeration value of type EPollResult indicating the result of the polling operation.
         */
        static ETest_PollResult PollTick(HANDLE Handle, unsigned char* Buffer, std::int32_t Length, DWORD& OutBytesRead)
        {
            std::int32_t Err = ERROR_SUCCESS;
            PingOnce(Handle, &Err);

            OutBytesRead = 0;
            if (!ReadFile(Handle, Buffer, Length, &OutBytesRead, nullptr))
            {
                return ETest_PollResult::Disconnected;
            }

            return ETest_PollResult::ReadOk;
        }

        /**
         * @brief Determines whether the given error code should be treated as a device disconnection.
         *
         * This method evaluates the provided error code and identifies if it corresponds
         * to a scenario where a device should be considered as disconnected.
         *
         * @param Error The error code to evaluate.
         * @return true if the error code indicates a device disconnection, false otherwise.
         */
        static bool ShouldTreatAsDisconnected(const std::int32_t Error)
        {
            switch (Error)
            {
            case ERROR_DEVICE_NOT_CONNECTED:
            case ERROR_GEN_FAILURE:
            case ERROR_INVALID_HANDLE:
            case ERROR_BAD_COMMAND:
            case ERROR_FILE_NOT_FOUND:
            case ERROR_ACCESS_DENIED:
                return true;
            default:
                return false;
            }
        }
    };
} // namespace o3de_dualsense
#endif