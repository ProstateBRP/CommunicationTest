/*=========================================================================

  Program:   BRP Prostate Robot: Testing Simulator (Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlClientSocket.h"
#include "ClientNormalOperationTest.h"
/*
int ExecNormalOperationTest(igtl::Socket* socket);
int ExecStartUpErrorTest(igtl::Socket* socket);
int ExecCalibrationErrorTest(igtl::Socket* socket);
int ExecTargetingWithoutCalibrationTest(igtl::Socket* socket);
int ExecOutOfRangeTest(igtl::Socket* socket);
int ExecStopTest(igtl::Socket* socket);
int ExecEmergencyTest(igtl::Socket* socket);
int ExecMoveToWithoutTargetTest(igtl::Socket* socket);
int ExecAccidentalMoveToTargetTest(igtl::Socket* socket);
int ExecHardwareErrorTest(igtl::Socket* socket);
*/

/*
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
int ReceiveMessageHeader(igtl::Socket* socket, igtl::MessageHeader* headerMsg, int timeout);
int SendStringMessage(igtl::Socket* socket, const char* name, const char* string);
int SendTransformMessage(igtl::Socket* socket, const char* name, igtl::Matrix4x4& matrix);

int CheckAndReceiveStringMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                 const char* name, const char* string);
int CheckAndReceiveStatusMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                 const char* name, int code);
int CheckAndReceiveTransformMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                    const char* name, igtl::Matrix4x4& matrix, double err = 1.0e-10);
*/

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <string>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <test>     : Test # (1-10)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  int    test     = atoi(argv[3]);

  if (test <= 0 || test > 10)
    {
    std::cerr << "Invalid test" << std::endl;
    exit(0);
    }


  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Call Test
  switch (test)
    {
    case 1:
      {
      ClientNormalOperationTest * cnotest = new ClientNormalOperationTest();
      cnotest->SetSocket(socket);
      cnotest->Exec();
      break;
      }
    default:
      break;
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();
}

/*
int ExecNormalOperationTest(igtl::Socket* socket)
{
  
  int queryCounter = 0;
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  
  SendStringMessage(socket, "CMD_0001", "START_UP");
  ReceiveMessageHeader(socket, headerMsg, 100);

  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0001", "START_UP"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, START_UP) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(socket, headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "START_UP", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(START_UP, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }

  SendStringMessage(socket, "CMD_0002", "PLANNING");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0002", "PLANNING"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, PLANNING) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage(socket, "CMD_0003", "CALIBRATION");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0003", "CALIBRATION"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, CALIBRATION) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }

  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  SendTransformMessage(socket, "CLB_0004", matrix);
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(socket, headerMsg, "ACK_0004", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(ACK) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  ReceiveMessageHeader(socket, headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "CALIBRATION", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(CALIBRATION, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }

  SendStringMessage(socket, "CMD_0005", "TARGETING");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0005", "TARGETING"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, TARGETING) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(socket, headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "TARGETING", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(TARGETING, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  GetRandomTestMatrix(matrix);
  SendTransformMessage(socket, "TGT_0006", matrix);
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(socket, headerMsg, "ACK_0006", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(ACK) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(socket, headerMsg, 10000);
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "TARGET", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(TARGETING, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(socket, headerMsg, "TARGET", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(TARGET) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage(socket, "CMD_0007", "MOVE_TO_TARGET");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0007", "MOVE_TO_TARGET"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, MOVE_TO_TARGET) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }

  int fCurrentPositionReceived = 0;
  for (;;) // Can receive more than 1 transform message
    {
    ReceiveMessageHeader(socket, headerMsg, 10000);
    if (strcmp(headerMsg->GetDeviceName(), "TRANSFORM") == 0)
      {
      if (!CheckAndReceiveTransformMessage(socket, headerMsg, "CURRENT_POSITION", matrix, -1))
        {
        std::cerr << "ERROR: Could not receive TRANSFORM(CURRENT_POSITION) message." << std::endl;
        socket->CloseSocket();
        return 0;
        }
      else
        {
        fCurrentPositionReceived = 1;
        }
      }
    }

  ReceiveMessageHeader(socket, headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "MOVE_TO_TARGET", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(MOVE_TO_TARGET, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveTransformMessage(socket, headerMsg, "CURRENT_POSITION", matrix))
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(CURRENT_POSITION) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  SendStringMessage(socket, "CMD_0008", "MANUAL");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0004", "MANUAL"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, MANUAL) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }

  ReceiveMessageHeader(socket, headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "MANUAL", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(MANUAL, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }

  //send GET_TRANSFORM(CURRENT_POSITION)
  //if (not receive TRANSFORM(CURRENT_POSITION, matrix8) within 10s) failure
  //if (matrix 8 does not match the current position of the robot) failure
  //
  //send GET_STATUS(CURRENT_STATUS)
  //if (not receive STATUS(XXXXX, Code:??:??) within 10s) failure

  SendStringMessage(socket, "CMD_0009", "STOP");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0004", "STOP"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, EMERGENCY) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(socket, headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "STOP", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(STOP, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }

  SendStringMessage(socket, "CMD_0010", "EMERGENCY");
  ReceiveMessageHeader(socket, headerMsg, 100);
  if (!CheckAndReceiveStringMessage(socket, headerMsg, "ACK_0010", "EMERGENCY"))
    {
    std::cerr << "ERROR: Could not receive STRING(ACK, EMERGENCY) message." << std::endl;
    socket->CloseSocket();
    return 0;
    }
  ReceiveMessageHeader(socket, headerMsg, 1000); // TODO: timeout is not valid
  if (!CheckAndReceiveStatusMessage(socket, headerMsg, "EMERGENCY", 1))
    {
    std::cerr << "ERROR: Could not receive STATUS(EMERGENCY, OK)" << std::endl;
    socket->CloseSocket();
    return 0;
    }
  
  return 1;
}

int ExecStartUpErrorTest(igtl::Socket* socket)
{
  return 1;
}

int ExecCalibrationErrorTest(igtl::Socket* socket)
{
  return 1;
}

int ExecTargetingWithoutCalibrationTest(igtl::Socket* socket)
{
  return 1;
}

int ExecOutOfRangeTest(igtl::Socket* socket)
{
  return 1;
}

int ExecStopTest(igtl::Socket* socket)
{
  return 1;
}

int ExecEmergencyTest(igtl::Socket* socket)
{
  return 1;
}

int ExecMoveToWithoutTargetTest(igtl::Socket* socket)
{
  return 1;
}

int ExecAccidentalMoveToTargetTest(igtl::Socket* socket)
{
  return 1;
}

int ExecHardwareErrorTest(igtl::Socket* socket)
{
  return 1;
}


*/
