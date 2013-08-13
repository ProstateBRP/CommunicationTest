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

#include <string.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"

#include "NavigationEmergencyStopDuringOperationTest.h"


NavigationEmergencyStopDuringOperationTest::NavigationEmergencyStopDuringOperationTest()
{
}

NavigationEmergencyStopDuringOperationTest::~NavigationEmergencyStopDuringOperationTest()
{
}

NavigationEmergencyStopDuringOperationTest::ErrorPointType NavigationEmergencyStopDuringOperationTest::Test()
{
  int queryCounter = 0;
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  std::cerr << "MESSAGE: ===== Step 1: START_UP =====" << std::endl;
  SendStringMessage("CMD_0001", "START_UP");
  ReceiveMessageHeader(headerMsg, this->TimeoutShort);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0001", "START_UP")) return Error(1,1);
  ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!CheckAndReceiveStatusMessage(headerMsg, "START_UP", 1)) return Error(1,2);

  std::cerr << "MESSAGE: ===== Step 2: PLANNING =====" << std::endl;
  SendStringMessage("CMD_0002", "PLANNING");
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0002", "PLANNING")) return Error(2,1);
  
  std::cerr << "MESSAGE: ===== Step 3: CALIBRATION =====" << std::endl;
  SendStringMessage("CMD_0003", "CALIBRATION");
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0003", "CALIBRATION")) return Error(3,1);

  igtl::Matrix4x4 matrix;
  //GetRandomTestMatrix(matrix);
  igtl::IdentityMatrix(matrix);
  SendTransformMessage("CLB_0004", matrix);
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0004", matrix)) return Error(3,2);
  // TODO: How can we differenciate Error(3,2) and Error(3,3)?
  
  ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!CheckAndReceiveStatusMessage(headerMsg, "CALIBRATION", 1)) return Error(3,4);

  std::cerr << "MESSAGE: ===== Step 4: TARGETING =====" << std::endl;
  SendStringMessage("CMD_0005", "TARGETING");
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0005", "TARGETING")) return Error(4,1);

  ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGETING", 1)) return Error(4,2);

  //GetRandomTestMatrix(matrix);
  igtl::IdentityMatrix(matrix);
  matrix[0][3] = 20; // 20 mm in R-L
  matrix[1][3] = 40; // 40 mm in A-P
  matrix[2][3] = 50; // 10 mm in S-I
  SendTransformMessage("TGT_0006", matrix);
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveTransformMessage(headerMsg, "ACK_0006", matrix)) return Error(4,3);
  ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!CheckAndReceiveStatusMessage(headerMsg, "TARGET", 1)) return Error(4,5);
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveTransformMessage(headerMsg, "TARGET", matrix)) return Error(4,6);
  
  std::cerr << "MESSAGE: ===== Step 5: MOVE_TO_TARGET =====" << std::endl;
  SendStringMessage("CMD_0007", "MOVE_TO_TARGET");
  ReceiveMessageHeader(headerMsg, this->TimeoutMedium);
  if (!CheckAndReceiveStringMessage(headerMsg, "ACK_0007", "MOVE_TO_TARGET")) return Error(5,1);

  std::cerr << "MESSAGE: ===== Step 6: Emergency while moving =====" << std::endl;  

  int fCurrentPositionReceived = 0;
  int count = 0;
  int fEmergencyAcknowledged = 0;
  int fEmergencyStatusConfirmed = 0;
  int timeout = this->TimeoutLong;

  for (;;) // Can receive more than 1 transform message
    {
    ReceiveMessageHeader(headerMsg, timeout);
    if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
      {
      if (!CheckAndReceiveTransformMessage(headerMsg, "CURRENT_POSITION", matrix, -1))
        {
        return Error(5,2);
        }
      else
        {
        fCurrentPositionReceived = 1;
        count ++;
        // after receiving current position 5 times, send emergency command
        if (count > 5)
          {
          SendStringMessage("CMD_0008", "EMERGENCY");
          // update timeout
          timeout = this->TimeoutShort;
          }
        }
      }
    else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
      {
      if (!CheckAndReceiveStringMessage(headerMsg, "CMD_0008", "EMERGENCY")) return Error(6,1);
      fEmergencyAcknowledged = 1;
      }
    else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
      {
      if (!fEmergencyAcknowledged) return Error(6,1);
      if (!CheckAndReceiveStatusMessage(headerMsg, "EMERGENCY", 1)) return Error(6,2);
      fEmergencyStatusConfirmed = 1;
      break;
      }
    else
      {
      break;
      }
    }

  if (!fEmergencyStatusConfirmed) return Error(6,2);
  
  return SUCCESS;
}
