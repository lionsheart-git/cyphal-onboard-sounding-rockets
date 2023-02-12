/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#ifndef SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
#define SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_

#include <gtest/gtest.h>

#include "Node.h"
#include "NodeFactory.h"


class ReferenceConfigurationTest : public ::testing::Test {

  protected:
    void SetUp() override;
    void TearDown() override;

    void HandleLoop();
    void WarmUp(float seconds = 10);

    NodeFactory node_factory_;

    std::unique_ptr<Node> flight_computer_;
    std::unique_ptr<Node> telemetry_;
    std::unique_ptr<Node> sensors_;
    std::unique_ptr<Node> payload_;

};

#endif //SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
