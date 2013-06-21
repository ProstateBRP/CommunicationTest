PROJECT(BRPProstateRobotTest)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(TestSimulatorServer  TestSimulatorServer.cxx)
TARGET_LINK_LIBRARIES(TestSimulatorServer OpenIGTLink)

ADD_EXECUTABLE(TestSimulatorClient  TestSimulatorClient.cxx)
TARGET_LINK_LIBRARIES(TestSimulatorClient OpenIGTLink)
