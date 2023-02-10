/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#ifndef SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
#define SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_

#include <gtest/gtest.h>

class ReferenceConfigurationTest : public ::testing::Test {

  protected:
    void SetUp() override;
    void TearDown() override;

};

#endif //SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
