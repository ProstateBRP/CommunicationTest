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
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0001", "START_UP")) return ERROR;
  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "START_UP", 1)) return ERROR;

  SendStringMessage("CMD_0002", "PLANNING");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0002", "PLANNING")) return ERROR;
  
  SendStringMessage("CMD_0003", "CALIBRATION");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0003", "CALIBRATION")) return ERROR;

  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  SendTransformMessage("CLB_0004", matrix);
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0004", matrix)) return ERROR;
  
  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "CALIBRATION", 1)) return ERROR;

  SendStringMessage("CMD_0005", "TARGETING");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0005", "TARGETING")) return ERROR;

  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGETING", 1)) return ERROR;
  
  GetRandomTestMatrix(matrix);
  SendTransformMessage("TGT_0006", matrix);
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0006", matrix)) return ERROR;
  ReceiveMessageHeader(headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGET", 1)) return ERROR;
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "TARGET", matrix)) return ERROR;
  
  SendStringMessage("CMD_0007", "MOVE_TO_TARGET");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0007", "MOVE_TO_TARGET")) return ERROR;

  int fCurrentPositionReceived = 0;
  for (;;) // Can receive more than 1 transform message
    {
    ReceiveMessageHeader(headerMsg, 10000);
    if (strcmp(headerMsg->GetDeviceName(), "TRANSFORM") == 0)
      {
      if (!CheckAndReceiveTransformMessage(headerMsg, "CURRENT_POSITION", matrix, -1))
        {
        return ERROR;
        }
      else
        {
        fCurrentPositionReceived = 1;
        }
      }
    }
  
  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "MOVE_TO_TARGET", 1)) return ERROR;

  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(headerMsg, "CURRENT_POSITION", matrix)) return ERROR;
  
  SendStringMessage("CMD_0008", "MANUAL");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0004", "MANUAL")) return ERROR;

  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "MANUAL", 1)) return ERROR;

  //send GET_TRANSFORM(CURRENT_POSITION)
  //if (not receive TRANSFORM(CURRENT_POSITION, matrix8) within 10s) failure
  //if (matrix 8 does not match the current position of the robot) failure
  //
  //send GET_STATUS(CURRENT_STATUS)
  //if (not receive STATUS(XXXXX, Code:??:??) within 10s) failure

  SendStringMessage("CMD_0009", "STOP");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0004", "STOP")) return ERROR;

  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "STOP", 1)) return ERROR;

  SendStringMessage("CMD_0010", "EMERGENCY");
  ReceiveMessageHeader(headerMsg, 100);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0010", "EMERGENCY")) return ERROR;

  ReceiveMessageHeader(headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(headerMsg, "EMERGENCY", 1)) return ERROR;
  
  return 1;
}
