// Copyright dSPACE GmbH. All rights reserved.

#include <string>

#include "CoSimTypes.h"
#include "Generator.h"
#include "Helper.h"
#include "Protocol.h"
#include "SocketChannel.h"
#include "TestHelper.h"

#ifdef _WIN32
#include "LocalChannel.h"
#endif

using namespace DsVeosCoSim;

namespace {

class TestProtocol : public testing::TestWithParam<ConnectionKind> {
protected:
    std::unique_ptr<Channel> _senderChannel;
    std::unique_ptr<Channel> _receiverChannel;

    void CustomSetUp(ConnectionKind connectionKind) {
        if (connectionKind == ConnectionKind::Remote) {
            TcpChannelServer server(0, true);
            uint16_t port = server.GetLocalPort();

            _senderChannel = std::make_unique<SocketChannel>(ConnectToTcpChannel("127.0.0.1", port));
            _receiverChannel = std::make_unique<SocketChannel>(Accept(server));
        } else {
#ifdef _WIN32
            const std::string name = GenerateString("LocalChannel名前");
            LocalChannelServer server(name);

            _senderChannel = std::make_unique<LocalChannel>(ConnectToLocalChannel(name));
            _receiverChannel = std::make_unique<LocalChannel>(Accept(server));
#else
            const std::string name = GenerateString("UdsChannel名前");
            UdsChannelServer server(name);

            _senderChannel = std::make_unique<SocketChannel>(ConnectToUdsChannel(name));
            _receiverChannel = std::make_unique<SocketChannel>(Accept(server));
#endif
        }
    }

    void TearDown() override {
        _senderChannel->Disconnect();
        _receiverChannel->Disconnect();
    }

    void AssertFrame(FrameKind expected) const {
        FrameKind frameKind{};
        ASSERT_TRUE(Protocol::ReceiveHeader(_receiverChannel->GetReader(), frameKind));
        ASSERT_EQ(static_cast<int32_t>(expected), static_cast<int32_t>(frameKind));
    }
};

INSTANTIATE_TEST_SUITE_P(,
                         TestProtocol,
                         testing::Values(ConnectionKind::Local, ConnectionKind::Remote),
                         [](const testing::TestParamInfo<ConnectionKind>& info) { return ToString(info.param); });

TEST_P(TestProtocol, SendAndReceiveOk) {
    // Arrange
    CustomSetUp(GetParam());

    // Act
    ASSERT_TRUE(Protocol::SendOk(_senderChannel->GetWriter()));

    // Assert
    AssertFrame(FrameKind::Ok);
}

TEST_P(TestProtocol, SendAndReceiveError) {
    // Arrange
    CustomSetUp(GetParam());

    const std::string sendErrorMessage = GenerateString("Errorメッセージ");

    // Act
    ASSERT_TRUE(Protocol::SendError(_senderChannel->GetWriter(), sendErrorMessage));

    // Assert
    AssertFrame(FrameKind::Error);

    std::string receiveErrorMessage;
    ASSERT_TRUE(Protocol::ReadError(_receiverChannel->GetReader(), receiveErrorMessage));
    AssertEq(sendErrorMessage, receiveErrorMessage);
}

TEST_P(TestProtocol, SendAndReceivePing) {
    // Arrange
    CustomSetUp(GetParam());

    // Act
    ASSERT_TRUE(Protocol::SendPing(_senderChannel->GetWriter()));

    // Assert
    AssertFrame(FrameKind::Ping);
}

TEST_P(TestProtocol, SendAndReceivePingOk) {
    // Arrange
    CustomSetUp(GetParam());

    const auto sendCommand = static_cast<Command>(GenerateU32());

    // Act
    ASSERT_TRUE(Protocol::SendPingOk(_senderChannel->GetWriter(), sendCommand));

    // Assert
    AssertFrame(FrameKind::PingOk);

    Command receiveCommand{};
    ASSERT_TRUE(Protocol::ReadPingOk(_receiverChannel->GetReader(), receiveCommand));
    ASSERT_EQ(sendCommand, receiveCommand);
}

TEST_P(TestProtocol, SendAndReceiveConnect) {
    // Arrange
    CustomSetUp(GetParam());

    const uint32_t sendVersion = GenerateU32();
    constexpr Mode sendMode{};
    const std::string sendServerName = GenerateString("Server名前");
    const std::string sendClientName = GenerateString("Client名前");

    // Act
    ASSERT_TRUE(
        Protocol::SendConnect(_senderChannel->GetWriter(), sendVersion, sendMode, sendServerName, sendClientName));

    // Assert
    AssertFrame(FrameKind::Connect);

    uint32_t receiveVersion{};
    Mode receiveMode{};
    std::string receiveServerName;
    std::string receiveClientName;
    ASSERT_TRUE(Protocol::ReadConnect(_receiverChannel->GetReader(),
                                      receiveVersion,
                                      receiveMode,
                                      receiveServerName,
                                      receiveClientName));
    ASSERT_EQ(sendVersion, receiveVersion);
    ASSERT_EQ(static_cast<int32_t>(sendMode), static_cast<int32_t>(receiveMode));
    AssertEq(sendServerName, receiveServerName);
    AssertEq(sendClientName, receiveClientName);
}

TEST_P(TestProtocol, SendAndReceiveConnectOk) {
    // Arrange
    CustomSetUp(GetParam());

    const uint32_t sendProtocolVersion = GenerateU32();
    constexpr Mode sendMode{};
    const DsVeosCoSim_SimulationTime sendStepSize = GenerateI64();
    constexpr SimulationState sendSimulationState{};
    const std::vector<IoSignal> sendIncomingSignals = CreateSignals(2);
    const std::vector<IoSignal> sendOutgoingSignals = CreateSignals(3);
    const std::vector<CanController> sendCanControllers = CreateCanControllers(4);
    const std::vector<EthController> sendEthControllers = CreateEthControllers(5);
    const std::vector<LinController> sendLinControllers = CreateLinControllers(6);

    // Act
    ASSERT_TRUE(Protocol::SendConnectOk(_senderChannel->GetWriter(),
                                        sendProtocolVersion,
                                        sendMode,
                                        sendStepSize,
                                        sendSimulationState,
                                        sendIncomingSignals,
                                        sendOutgoingSignals,
                                        sendCanControllers,
                                        sendEthControllers,
                                        sendLinControllers));

    // Assert
    AssertFrame(FrameKind::ConnectOk);

    uint32_t receiveProtocolVersion{};
    Mode receiveMode{};
    DsVeosCoSim_SimulationTime receiveStepSize{};
    SimulationState receiveSimulationState{};
    std::vector<IoSignal> receiveIncomingSignals;
    std::vector<IoSignal> receiveOutgoingSignals;
    std::vector<CanController> receiveCanControllers;
    std::vector<EthController> receiveEthControllers;
    std::vector<LinController> receiveLinControllers;
    ASSERT_TRUE(Protocol::ReadConnectOk(_receiverChannel->GetReader(),
                                        receiveProtocolVersion,
                                        receiveMode,
                                        receiveStepSize,
                                        receiveSimulationState,
                                        receiveIncomingSignals,
                                        receiveOutgoingSignals,
                                        receiveCanControllers,
                                        receiveEthControllers,
                                        receiveLinControllers));
    ASSERT_EQ(sendProtocolVersion, receiveProtocolVersion);
    ASSERT_EQ(static_cast<int32_t>(sendMode), static_cast<int32_t>(receiveMode));
    ASSERT_EQ(sendStepSize, receiveStepSize);
    AssertEq(sendIncomingSignals, receiveIncomingSignals);
    AssertEq(sendOutgoingSignals, receiveOutgoingSignals);
    AssertEq(sendCanControllers, receiveCanControllers);
    AssertEq(sendEthControllers, receiveEthControllers);
    AssertEq(sendLinControllers, receiveLinControllers);
}

TEST_P(TestProtocol, SendAndReceiveStart) {
    // Arrange
    CustomSetUp(GetParam());

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    // Act
    ASSERT_TRUE(Protocol::SendStart(_senderChannel->GetWriter(), sendSimulationTime));

    // Assert
    AssertFrame(FrameKind::Start);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    ASSERT_TRUE(Protocol::ReadStart(_receiverChannel->GetReader(), receiveSimulationTime));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveStop) {
    // Arrange
    CustomSetUp(GetParam());

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    // Act
    ASSERT_TRUE(Protocol::SendStop(_senderChannel->GetWriter(), sendSimulationTime));

    // Assert
    AssertFrame(FrameKind::Stop);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    ASSERT_TRUE(Protocol::ReadStop(_receiverChannel->GetReader(), receiveSimulationTime));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveTerminate) {
    // Arrange
    CustomSetUp(GetParam());

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();
    const DsVeosCoSim_TerminateReason sendTerminateReason =
        GenerateRandom(DsVeosCoSim_TerminateReason_Finished, DsVeosCoSim_TerminateReason_Error);

    // Act
    ASSERT_TRUE(Protocol::SendTerminate(_senderChannel->GetWriter(), sendSimulationTime, sendTerminateReason));

    // Assert
    AssertFrame(FrameKind::Terminate);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    DsVeosCoSim_TerminateReason receiveTerminateReason{};
    ASSERT_TRUE(Protocol::ReadTerminate(_receiverChannel->GetReader(), receiveSimulationTime, receiveTerminateReason));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
    ASSERT_EQ(sendTerminateReason, receiveTerminateReason);
}

TEST_P(TestProtocol, SendAndReceivePause) {
    // Arrange
    CustomSetUp(GetParam());

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    // Act
    ASSERT_TRUE(Protocol::SendPause(_senderChannel->GetWriter(), sendSimulationTime));

    // Assert
    AssertFrame(FrameKind::Pause);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    ASSERT_TRUE(Protocol::ReadPause(_receiverChannel->GetReader(), receiveSimulationTime));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveContinue) {
    // Arrange
    CustomSetUp(GetParam());

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    // Act
    ASSERT_TRUE(Protocol::SendContinue(_senderChannel->GetWriter(), sendSimulationTime));

    // Assert
    AssertFrame(FrameKind::Continue);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    ASSERT_TRUE(Protocol::ReadContinue(_receiverChannel->GetReader(), receiveSimulationTime));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveStep) {
    // Arrange
    ConnectionKind connectionKind = GetParam();
    CustomSetUp(connectionKind);

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    const std::string ioBufferName = GenerateString("IoBuffer名前");
    IoBuffer clientIoBuffer(CoSimType::Client, connectionKind, ioBufferName, {}, {});
    IoBuffer serverIoBuffer(CoSimType::Server, connectionKind, ioBufferName, {}, {});

    const std::string busBufferName = GenerateString("BusBuffer名前");
    BusBuffer clientBusBuffer(CoSimType::Client, connectionKind, ioBufferName, {}, {}, {});
    BusBuffer serverBusBuffer(CoSimType::Server, connectionKind, ioBufferName, {}, {}, {});

    // Act
    ASSERT_TRUE(Protocol::SendStep(_senderChannel->GetWriter(), sendSimulationTime, clientIoBuffer, clientBusBuffer));

    // Assert
    AssertFrame(FrameKind::Step);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    ASSERT_TRUE(
        Protocol::ReadStep(_receiverChannel->GetReader(), receiveSimulationTime, serverIoBuffer, serverBusBuffer, {}));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveStepOk) {
    // Arrange
    ConnectionKind connectionKind = GetParam();
    CustomSetUp(connectionKind);

    const DsVeosCoSim_SimulationTime sendSimulationTime = GenerateI64();

    const auto sendCommand = static_cast<Command>(GenerateU32());

    const std::string ioBufferName = GenerateString("IoBuffer名前");
    IoBuffer clientIoBuffer(CoSimType::Client, connectionKind, ioBufferName, {}, {});
    IoBuffer serverIoBuffer(CoSimType::Server, connectionKind, ioBufferName, {}, {});

    const std::string busBufferName = GenerateString("BusBuffer名前");
    BusBuffer clientBusBuffer(CoSimType::Client, connectionKind, ioBufferName, {}, {}, {});
    BusBuffer serverBusBuffer(CoSimType::Server, connectionKind, ioBufferName, {}, {}, {});

    // Act
    ASSERT_TRUE(Protocol::SendStepOk(_senderChannel->GetWriter(),
                                     sendSimulationTime,
                                     sendCommand,
                                     clientIoBuffer,
                                     clientBusBuffer));

    // Assert
    AssertFrame(FrameKind::StepOk);

    DsVeosCoSim_SimulationTime receiveSimulationTime{};
    Command receiveCommand{};
    ASSERT_TRUE(Protocol::ReadStepOk(_receiverChannel->GetReader(),
                                     receiveSimulationTime,
                                     receiveCommand,
                                     serverIoBuffer,
                                     serverBusBuffer,
                                     {}));
    ASSERT_EQ(sendSimulationTime, receiveSimulationTime);
}

TEST_P(TestProtocol, SendAndReceiveGetPort) {
    // Arrange
    CustomSetUp(GetParam());

    const std::string sendServerName = GenerateString("Server名前");

    // Act
    ASSERT_TRUE(Protocol::SendGetPort(_senderChannel->GetWriter(), sendServerName));

    // Assert
    AssertFrame(FrameKind::GetPort);

    std::string receiveServerName;
    ASSERT_TRUE(Protocol::ReadGetPort(_receiverChannel->GetReader(), receiveServerName));
    AssertEq(sendServerName, receiveServerName);
}

TEST_P(TestProtocol, SendAndReceiveGetPortOk) {
    // Arrange
    CustomSetUp(GetParam());

    const uint16_t sendPort = GenerateU16();

    // Act
    ASSERT_TRUE(Protocol::SendGetPortOk(_senderChannel->GetWriter(), sendPort));

    // Assert
    AssertFrame(FrameKind::GetPortOk);

    uint16_t receivePort{};
    ASSERT_TRUE(Protocol::ReadGetPortOk(_receiverChannel->GetReader(), receivePort));
    ASSERT_EQ(sendPort, receivePort);
}

TEST_P(TestProtocol, SendAndReceiveSetPort) {
    // Arrange
    CustomSetUp(GetParam());

    const std::string sendServerName = GenerateString("Server名前");
    const uint16_t sendPort = GenerateU16();

    // Act
    ASSERT_TRUE(Protocol::SendSetPort(_senderChannel->GetWriter(), sendServerName, sendPort));

    // Assert
    AssertFrame(FrameKind::SetPort);

    std::string receiveServerName;
    uint16_t receivePort{};
    ASSERT_TRUE(Protocol::ReadSetPort(_receiverChannel->GetReader(), receiveServerName, receivePort));
    AssertEq(sendServerName, receiveServerName);
    ASSERT_EQ(sendPort, receivePort);
}

TEST_P(TestProtocol, SendAndReceiveUnsetPort) {
    // Arrange
    CustomSetUp(GetParam());

    const std::string sendServerName = GenerateString("Server名前");

    // Act
    ASSERT_TRUE(Protocol::SendUnsetPort(_senderChannel->GetWriter(), sendServerName));

    // Assert
    AssertFrame(FrameKind::UnsetPort);

    std::string receiveServerName;
    ASSERT_TRUE(Protocol::ReadUnsetPort(_receiverChannel->GetReader(), receiveServerName));
    AssertEq(sendServerName, receiveServerName);
}

}  // namespace
