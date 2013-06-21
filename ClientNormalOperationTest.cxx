/*=========================================================================

  Program:   BRP Prostate Robot: Testing Simulator (Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  Please see
    http://wiki.na-mic.org/Wiki/index.php/ProstateBRP_OpenIGTLink_Communication_June_2013
  for the detail of the testing protocol.

=========================================================================*/

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"

#include "ClientNormalOperationTest.h"


ClientNormalOperationTest::ClientNormalOperationTest()
{
}

ClientNormalOperationTest::~ClientNormalOperationTest()
{
}

int ClientNormalOperationTest::Test()
{
  int queryCounter = 0;
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  
  SendStringMessage("CMD_0001", "START_UP");
  ReceiveMessageHeader(headerMsg, 100);

  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0001", "START_UP"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, START_UP) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "START_UP", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(START_UP, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  SendStringMessage("CMD_0002", "PLANNING");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0002", "PLANNING"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, PLANNING) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage("CMD_0003", "CALIBRATION");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0003", "CALIBRATION"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, CALIBRATION) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  SendTransformMessage("CLB_0004", matrix);
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0004", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(ACK) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "CALIBRATION", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(CALIBRATION, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  SendStringMessage("CMD_0005", "TARGETING");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0005", "TARGETING"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, TARGETING) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGETING", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(TARGETING, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  GetRandomTestMatrix(matrix);
  SendTransformMessage("TGT_0006", matrix);
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0006", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(ACK) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGET", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(TARGETING, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "TARGET", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(TARGET) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage("CMD_0007", "MOVE_TO_TARGET");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0007", "MOVE_TO_TARGET"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, MOVE_TO_TARGET) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  int fCurrentPositionReceived = 0;
  for (;;) // Can receive more than 1 transform message
    {
    ReceiveMessageHeader(headerMsg, 10000);
    if (strcmp(headerMsg->GetDeviceName(), "TRANSFORM") == 0)
      {
      if (!CheckAndReceiveTransformMessage(headerMsg, "CURRENT_POSITION", matrix, -1))
        {
        std::cerr << "ERROR: Could not receive TRANSFORM(CURRENT_POSITION) message." << std::endl;
        this->Socket->CloseSocket();
        return 0;
        }
      else
        {
        fCurrentPositionReceived = 1;
        }
      }
    }

  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "MOVE_TO_TARGET", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(MOVE_TO_TARGET, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "CURRENT_POSITION", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(CURRENT_POSITION) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage("CMD_0008", "MANUAL");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0004", "MANUAL"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, MANUAL) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "MANUAL", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(MANUAL, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  //send GET_TRANSFORM(CURRENT_POSITION)
  //if (not receive TRANSFORM(CURRENT_POSITION, matrix8) within 10s) failure
  //if (matrix 8 does not match the current position of the robot) failure
  //
  //send GET_STATUS(CURRENT_STATUS)
  //if (not receive STATUS(XXXXX, Code:??:??) within 10s) failure

  SendStringMessage("CMD_0009", "STOP");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0004", "STOP"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, EMERGENCY) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "STOP", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(STOP, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }

  SendStringMessage("CMD_0010", "EMERGENCY");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0010", "EMERGENCY"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, EMERGENCY) message." << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "EMERGENCY", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(EMERGENCY, OK)" << std::endl;
    this->Socket->CloseSocket();
    return 0;
    }
  
  return 1;
}
