// Copyright dSPACE GmbH. All rights reserved.

#pragma once

#include <gtest/gtest.h>
#include <vector>

#include "CoSimTypes.h"

DsVeosCoSim::CoSimType GetCounterPart(DsVeosCoSim::CoSimType coSimType);
std::string GetCounterPart(const std::string& name, DsVeosCoSim::ConnectionKind connectionKind);

void AssertByteArray(const void* expected, const void* actual, size_t size);

void AssertLastMessage(std::string_view message);

void AssertEq(const DsVeosCoSim::IoSignal& expected, const DsVeosCoSim::IoSignal& actual);
void AssertEq(const DsVeosCoSim_CanController& expected, const DsVeosCoSim_CanController& actual);
void AssertEq(const DsVeosCoSim_EthController& expected, const DsVeosCoSim_EthController& actual);
void AssertEq(const DsVeosCoSim_LinController& expected, const DsVeosCoSim_LinController& actual);
void AssertEq(const DsVeosCoSim_CanMessage& expected, const DsVeosCoSim_CanMessage& actual);
void AssertEq(const DsVeosCoSim_EthMessage& expected, const DsVeosCoSim_EthMessage& actual);
void AssertEq(const DsVeosCoSim_LinMessage& expected, const DsVeosCoSim_LinMessage& actual);
void AssertEq(const std::string& expected, const std::string& actual);
void AssertEq(const char* expected, const char* actual);

template <typename T>
void AssertEq(const std::vector<T>& expected, const std::vector<T>& actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (size_t i = 0; i < expected.size(); i++) {
        AssertEq(expected[i], actual[i]);
    }
}
