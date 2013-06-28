PROJECT(BRPProstateRobotTest)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

set(NAVIGATION_SIM_SOURCE
  CommunicationTestBase.cxx
  ClientTestBase.cxx
  ClientNormalOperationTest.cxx
  TestSimulatorClient.cxx
)

set(ROBOT_SIM_SOURCE
)

#ADD_EXECUTABLE(TestSimulatorServer  ${ROBOT_SIM_SOURCE)
#TARGET_LINK_LIBRARIES(TestSimulatorServer OpenIGTLink)

ADD_EXECUTABLE(TestSimulatorClient ${NAVIGATION_SIM_SOURCE})
TARGET_LINK_LIBRARIES(TestSimulatorClient OpenIGTLink)
