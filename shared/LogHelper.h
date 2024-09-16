// Copyright dSPACE GmbH. All rights reserved.

#pragma once

#include "CoSimTypes.h"

void InitializeOutput();

void OnLogCallback(DsVeosCoSim::Severity severity, std::string_view message);

void LogIoSignal(const DsVeosCoSim_IoSignal& ioSignal);

void LogIoData(DsVeosCoSim::SimulationTime simulationTime,
               const DsVeosCoSim_IoSignal& ioSignal,
               uint32_t length,
               const void* value);

void LogCanController(const DsVeosCoSim_CanController& controller);
void LogEthController(const DsVeosCoSim_EthController& controller);
void LogLinController(const DsVeosCoSim_LinController& controller);

void LogCanMessage(DsVeosCoSim::SimulationTime simulationTime,
                   const DsVeosCoSim_CanController& controller,
                   const DsVeosCoSim_CanMessage& message);
void LogEthMessage(DsVeosCoSim::SimulationTime simulationTime,
                   const DsVeosCoSim_EthController& controller,
                   const DsVeosCoSim_EthMessage& message);
void LogLinMessage(DsVeosCoSim::SimulationTime simulationTime,
                   const DsVeosCoSim_LinController& controller,
                   const DsVeosCoSim_LinMessage& message);

void ClearLastMessage();
[[nodiscard]] std::string GetLastMessage();
