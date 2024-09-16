// Copyright dSPACE GmbH. All rights reserved.

#pragma once

#include <memory.h>
#include <array>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "DsVeosCoSim/DsVeosCoSim.h"

namespace DsVeosCoSim {

constexpr uint32_t CanMessageMaxLength = DSVEOSCOSIM_CAN_MESSAGE_MAX_LENGTH;  // NOLINT
constexpr uint32_t EthMessageMaxLength = DSVEOSCOSIM_ETH_MESSAGE_MAX_LENGTH;  // NOLINT
constexpr uint32_t LinMessageMaxLength = DSVEOSCOSIM_LIN_MESSAGE_MAX_LENGTH;  // NOLINT
constexpr uint32_t EthAddressLength = DSVEOSCOSIM_ETH_ADDRESS_LENGTH;

using SimulationTime = DsVeosCoSim_SimulationTime;

[[nodiscard]] inline double SimulationTimeToSeconds(SimulationTime simulationTime) {
    return DSVEOSCOSIM_SIMULATION_TIME_TO_SECONDS(simulationTime);
}

class CoSimException final : public std::runtime_error {
public:
    explicit CoSimException(std::string_view message) : std::runtime_error(message.data()) {
    }
};

enum class CoSimType {
    Client,
    Server
};

[[nodiscard]] inline std::string ToString(CoSimType coSimType) {
    switch (coSimType) {
        case CoSimType::Client:
            return "Client";
        case CoSimType::Server:
            return "Server";
    }

    return std::to_string(static_cast<int32_t>(coSimType));
}

enum class ConnectionKind {
    Remote,
    Local
};

[[nodiscard]] inline std::string ToString(ConnectionKind connectionKind) {
    switch (connectionKind) {
        case ConnectionKind::Remote:
            return "Remote";
        case ConnectionKind::Local:
            return "Local";
    }

    return std::to_string(static_cast<int32_t>(connectionKind));
}

enum class Command {
    None = DsVeosCoSim_Command_None,
    Step = DsVeosCoSim_Command_Step,
    Start = DsVeosCoSim_Command_Start,
    Stop = DsVeosCoSim_Command_Stop,
    Terminate = DsVeosCoSim_Command_Terminate,
    Pause = DsVeosCoSim_Command_Pause,
    Continue = DsVeosCoSim_Command_Continue,
    TerminateFinished,
    Ping
};

[[nodiscard]] inline std::string ToString(Command command) {
    switch (command) {
        case Command::None:
            return "None";
        case Command::Step:
            return "Step";
        case Command::Start:
            return "Start";
        case Command::Stop:
            return "Stop";
        case Command::Terminate:
            return "Terminate";
        case Command::Pause:
            return "Pause";
        case Command::Continue:
            return "Continue";
        case Command::TerminateFinished:
            return "TerminateFinished";
        case Command::Ping:
            return "Ping";
    }

    return std::to_string(static_cast<int32_t>(command));
}

enum class Severity {
    Error = DsVeosCoSim_Severity_Error,
    Warning = DsVeosCoSim_Severity_Warning,
    Info = DsVeosCoSim_Severity_Info,
    Trace = DsVeosCoSim_Severity_Trace
};

[[nodiscard]] inline std::string ToString(Severity severity) {
    switch (severity) {
        case Severity::Error:
            return "Error";
        case Severity::Warning:
            return "Warning";
        case Severity::Info:
            return "Info";
        case Severity::Trace:
            return "Trace";
    }

    return std::to_string(static_cast<int32_t>(severity));
}

enum class TerminateReason {
    Finished = DsVeosCoSim_TerminateReason_Finished,
    Error = DsVeosCoSim_TerminateReason_Error
};

inline std::string ToString(TerminateReason terminateReason) {
    switch (terminateReason) {
        case TerminateReason::Finished:
            return "Finished";
        case TerminateReason::Error:
            return "Error";
    }

    return std::to_string(static_cast<int32_t>(terminateReason));
}

enum class ConnectionState {
    Connected = DsVeosCoSim_ConnectionState_Connected,
    Disconnected = DsVeosCoSim_ConnectionState_Disconnected
};

inline std::string ToString(ConnectionState connectionState) {
    switch (connectionState) {
        case ConnectionState::Connected:
            return "Connected";
        case ConnectionState::Disconnected:
            return "Disconnected";
    }

    return std::to_string(static_cast<int32_t>(connectionState));
}

[[nodiscard]] inline size_t GetDataTypeSize(DsVeosCoSim_DataType dataType) {
    switch (dataType) {
        case DsVeosCoSim_DataType_Bool:
        case DsVeosCoSim_DataType_Int8:
        case DsVeosCoSim_DataType_UInt8:
            return 1;
        case DsVeosCoSim_DataType_Int16:
        case DsVeosCoSim_DataType_UInt16:
            return 2;
        case DsVeosCoSim_DataType_Int32:
        case DsVeosCoSim_DataType_UInt32:
        case DsVeosCoSim_DataType_Float32:
            return 4;
        case DsVeosCoSim_DataType_Int64:
        case DsVeosCoSim_DataType_UInt64:
        case DsVeosCoSim_DataType_Float64:
            return 8;
        case DsVeosCoSim_DataType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }

    return 0;
}

inline std::string ToString(DsVeosCoSim_DataType dataType) {
    switch (dataType) {
        case DsVeosCoSim_DataType_Bool:
            return "Bool";
        case DsVeosCoSim_DataType_Int8:
            return "Int8";
        case DsVeosCoSim_DataType_Int16:
            return "Int16";
        case DsVeosCoSim_DataType_Int32:
            return "Int32";
        case DsVeosCoSim_DataType_Int64:
            return "Int64";
        case DsVeosCoSim_DataType_UInt8:
            return "UInt8";
        case DsVeosCoSim_DataType_UInt16:
            return "UInt16";
        case DsVeosCoSim_DataType_UInt32:
            return "UInt32";
        case DsVeosCoSim_DataType_UInt64:
            return "UInt64";
        case DsVeosCoSim_DataType_Float32:
            return "Float32";
        case DsVeosCoSim_DataType_Float64:
            return "Float64";
        case DsVeosCoSim_DataType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }

    return std::to_string(dataType);
}

inline std::string ToString(DsVeosCoSim_SizeKind sizeKind) {
    switch (sizeKind) {
        case DsVeosCoSim_SizeKind_Fixed:
            return "Fixed";
        case DsVeosCoSim_SizeKind_Variable:
            return "Variable";
        case DsVeosCoSim_SizeKind_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }

    return std::to_string(sizeKind);
}

inline std::string ToString(DsVeosCoSim_LinControllerType type) {
    switch (type) {
        case DsVeosCoSim_LinControllerType_Responder:
            return "Responder";
        case DsVeosCoSim_LinControllerType_Commander:
            return "Commander";
        case DsVeosCoSim_LinControllerType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }

    return std::to_string(type);
}

enum class SimulationState {
};

enum class Mode {
};

using CanMessageFlags = DsVeosCoSim_CanMessageFlags;

[[nodiscard]] inline std::string CanMessageFlagsToString(CanMessageFlags flags) {
    std::string flagsStr;

    if ((flags & DsVeosCoSim_CanMessageFlags_Loopback) != 0) {
        flagsStr += ",Loopback";
    }

    if ((flags & DsVeosCoSim_CanMessageFlags_Error) != 0) {
        flagsStr += ",Error";
    }

    if ((flags & DsVeosCoSim_CanMessageFlags_Drop) != 0) {
        flagsStr += ",Drop";
    }

    if ((flags & DsVeosCoSim_CanMessageFlags_ExtendedId) != 0) {
        flagsStr += ",ExtendedId";
    }

    if ((flags & DsVeosCoSim_CanMessageFlags_BitRateSwitch) != 0) {
        flagsStr += ",BitRateSwitch";
    }

    if ((flags & DsVeosCoSim_CanMessageFlags_FlexibleDataRateFormat) != 0) {
        flagsStr += ",FlexibleDataRateFormat";
    }

    if (!flagsStr.empty()) {
        flagsStr.erase(0, 1);
    }

    return flagsStr;
}

using EthMessageFlags = DsVeosCoSim_EthMessageFlags;

[[nodiscard]] inline std::string EthMessageFlagsToString(EthMessageFlags flags) {
    std::string flagsStr;

    if ((flags & DsVeosCoSim_EthMessageFlags_Loopback) != 0) {
        flagsStr += ",Loopback";
    }

    if ((flags & DsVeosCoSim_EthMessageFlags_Error) != 0) {
        flagsStr += ",Error";
    }

    if ((flags & DsVeosCoSim_EthMessageFlags_Drop) != 0) {
        flagsStr += ",Drop";
    }

    if (!flagsStr.empty()) {
        flagsStr.erase(0, 1);
    }

    return flagsStr;
}

using LinMessageFlags = DsVeosCoSim_LinMessageFlags;

[[nodiscard]] inline std::string LinMessageFlagsToString(LinMessageFlags flags) {
    std::string flagsStr;

    if ((flags & DsVeosCoSim_LinMessageFlags_Loopback) != 0) {
        flagsStr += ",Loopback";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_Error) != 0) {
        flagsStr += ",Error";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_Drop) != 0) {
        flagsStr += ",Drop";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_Header) != 0) {
        flagsStr += ",Header";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_Response) != 0) {
        flagsStr += ",Response";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_WakeEvent) != 0) {
        flagsStr += ",WakeEvent";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_SleepEvent) != 0) {
        flagsStr += ",SleepEvent";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_EnhancedChecksum) != 0) {
        flagsStr += ",EnhancedChecksum";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_TransferOnce) != 0) {
        flagsStr += ",TransferOnce";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_ParityFailure) != 0) {
        flagsStr += ",ParityFailure";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_Collision) != 0) {
        flagsStr += ",Collision";
    }

    if ((flags & DsVeosCoSim_LinMessageFlags_NoResponse) != 0) {
        flagsStr += ",NoResponse";
    }

    if (!flagsStr.empty()) {
        flagsStr.erase(0, 1);
    }

    return flagsStr;
}

using IoSignalId = DsVeosCoSim_IoSignalId;

struct IoSignal {
    IoSignalId id{};
    uint32_t length{};
    DsVeosCoSim_DataType dataType{};
    DsVeosCoSim_SizeKind sizeKind{};
    std::string name;

    [[nodiscard]] operator DsVeosCoSim_IoSignal() const {
        DsVeosCoSim_IoSignal signal{};
        signal.id = id;
        signal.length = length;
        signal.dataType = dataType;
        signal.sizeKind = sizeKind;
        signal.name = name.c_str();
        return signal;
    }
};

[[nodiscard]] inline std::vector<DsVeosCoSim_IoSignal> Convert(const std::vector<IoSignal>& signals) {
    std::vector<DsVeosCoSim_IoSignal> ioSignals;
    ioSignals.reserve(signals.size());

    for (const auto& signal : signals) {
        ioSignals.push_back(signal);
    }

    return ioSignals;
}

using BusControllerId = DsVeosCoSim_BusControllerId;

struct CanController {
    BusControllerId id{};
    uint32_t queueSize{};
    uint64_t bitsPerSecond{};
    uint64_t flexibleDataRateBitsPerSecond{};
    std::string name;
    std::string channelName;
    std::string clusterName;

    [[nodiscard]] operator DsVeosCoSim_CanController() const {
        DsVeosCoSim_CanController controller{};
        controller.id = id;
        controller.queueSize = queueSize;
        controller.bitsPerSecond = bitsPerSecond;
        controller.flexibleDataRateBitsPerSecond = flexibleDataRateBitsPerSecond;
        controller.name = name.c_str();
        controller.channelName = channelName.c_str();
        controller.clusterName = clusterName.c_str();
        return controller;
    }
};

[[nodiscard]] inline std::vector<DsVeosCoSim_CanController> Convert(const std::vector<CanController>& controllers) {
    std::vector<DsVeosCoSim_CanController> canControllers;
    canControllers.reserve(controllers.size());

    for (const auto& controller : controllers) {
        canControllers.push_back(controller);
    }

    return canControllers;
}

struct EthController {
    BusControllerId id{};
    uint32_t queueSize{};
    uint64_t bitsPerSecond{};
    std::array<uint8_t, EthAddressLength> macAddress{};
    std::string name;
    std::string channelName;
    std::string clusterName;

    [[nodiscard]] operator DsVeosCoSim_EthController() const {
        DsVeosCoSim_EthController controller{};
        controller.id = id;
        controller.queueSize = queueSize;
        controller.bitsPerSecond = bitsPerSecond;
        (void)::memcpy(controller.macAddress, macAddress.data(), EthAddressLength);
        controller.name = name.c_str();
        controller.channelName = channelName.c_str();
        controller.clusterName = clusterName.c_str();
        return controller;
    }
};

[[nodiscard]] inline std::vector<DsVeosCoSim_EthController> Convert(const std::vector<EthController>& controllers) {
    std::vector<DsVeosCoSim_EthController> ethControllers;
    ethControllers.reserve(controllers.size());

    for (const auto& controller : controllers) {
        ethControllers.push_back(controller);
    }

    return ethControllers;
}

struct LinController {
    BusControllerId id{};
    uint32_t queueSize{};
    uint64_t bitsPerSecond{};
    DsVeosCoSim_LinControllerType type{};
    std::string name;
    std::string channelName;
    std::string clusterName;

    [[nodiscard]] operator DsVeosCoSim_LinController() const {
        DsVeosCoSim_LinController controller{};
        controller.id = id;
        controller.queueSize = queueSize;
        controller.bitsPerSecond = bitsPerSecond;
        controller.type = type;
        controller.name = name.c_str();
        controller.channelName = channelName.c_str();
        controller.clusterName = clusterName.c_str();
        return controller;
    }
};

[[nodiscard]] inline std::vector<DsVeosCoSim_LinController> Convert(const std::vector<LinController>& controllers) {
    std::vector<DsVeosCoSim_LinController> linControllers;
    linControllers.reserve(controllers.size());

    for (const auto& controller : controllers) {
        linControllers.push_back(controller);
    }

    return linControllers;
}

using LogCallback = std::function<void(Severity, std::string_view)>;

using SimulationCallback = std::function<void(SimulationTime simulationTime)>;
using SimulationTerminatedCallback = std::function<void(SimulationTime simulationTime, TerminateReason reason)>;
using IncomingSignalChangedCallback = std::function<
    void(SimulationTime simulationTime, const DsVeosCoSim_IoSignal& ioSignal, uint32_t length, const void* value)>;
using CanMessageReceivedCallback = std::function<void(SimulationTime simulationTime,
                                                      const DsVeosCoSim_CanController& controller,
                                                      const DsVeosCoSim_CanMessage& message)>;
using EthMessageReceivedCallback = std::function<void(SimulationTime simulationTime,
                                                      const DsVeosCoSim_EthController& controller,
                                                      const DsVeosCoSim_EthMessage& message)>;
using LinMessageReceivedCallback = std::function<void(SimulationTime simulationTime,
                                                      const DsVeosCoSim_LinController& controller,
                                                      const DsVeosCoSim_LinMessage& message)>;

struct Callbacks {
    DsVeosCoSim_Callbacks callbacks;
    SimulationCallback simulationStartedCallback;
    SimulationCallback simulationStoppedCallback;
    SimulationTerminatedCallback simulationTerminatedCallback;
    SimulationCallback simulationPausedCallback;
    SimulationCallback simulationContinuedCallback;
    SimulationCallback simulationBeginStepCallback;
    SimulationCallback simulationEndStepCallback;
    IncomingSignalChangedCallback incomingSignalChangedCallback;
    CanMessageReceivedCallback canMessageReceivedCallback;
    LinMessageReceivedCallback linMessageReceivedCallback;
    EthMessageReceivedCallback ethMessageReceivedCallback;
};

struct ConnectConfig {
    std::string remoteIpAddress;
    std::string serverName;
    std::string clientName;
    uint16_t remotePort{};
    uint16_t localPort{};
};

}  // namespace DsVeosCoSim
