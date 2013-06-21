/*=========================================================================

  Program:   OpenIGTLink -- Example for String Message Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"

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
      ExecNormalOperationTest(socket);
      break;
    case 2:
      ExecStartUpErrorTest(socket);
      break;
    case 3:
      ExecCalibrationErrorTest(socket);
      break;
    case 4:
      ExecTargetingWithoutCalibrationTest(socket);
      break;
    case 5:
      ExecOutOfRangeTest(socket);
      break;
    case 6:
      ExecStopTest(socket);
      break;
    case 7:
      ExecEmergencyTest(socket);
      break;
    case 8:
      ExecMoveToWithoutTargetTest(socket);
      break;
    case 9:
      ExecAccidentalMoveToTargetTest(socket);
      break;
    case 10:
      ExecHardwareErrorTest(socket);
      break;
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();
}


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


int ReceiveMessageHeader(igtl::Socket* socket, igtl::MessageHeader* headerMsg, int timeout)
{
  socket->SetTimeout(timeout);

  headerMsg->InitPack();
  int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
  if (r != headerMsg->GetPackSize())
    {
    if (r < 0) // Timeout
      {
      std::cerr << "ERROR: Timeout." << std::endl;
      socket->CloseSocket();
      exit(0);
      }
    else
      {
      std::cerr << "ERROR: Receiving message." << std::endl;
      socket->CloseSocket();
      exit(0);
      }
    }
  headerMsg->Unpack();
  return 1;
}


int CheckAndReceiveStringMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                 const char* name, const char* string)
{
  
  if (strcmp(headerMsg->GetDeviceType(), "STRING") != 0)
    {
    return 0;
    }

  // Create a message buffer to receive transform data
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetMessageHeader(headerMsg);
  stringMsg->AllocatePack();

  // Receive string data from the socket
  socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = stringMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    if (strcmp(stringMsg->GetDeviceName(), name) == 0 &&
        stringMsg->GetEncoding() == 3 &&
        strcmp(stringMsg->GetString(), string) == 0)
      {
      return 1;
      }
    }

  return 0;
}


int CheckAndReceiveStatusMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                 const char* name, int code)
{
  
  if (strcmp(headerMsg->GetDeviceType(), "STATUS") != 0)
    {
    return 0;
    }

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(headerMsg);
  statusMsg->AllocatePack();

  // Receive status data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    if (strcmp(statusMsg->GetDeviceName(), name) == 0 &&
        statusMsg->GetCode() == code)
      {
      return 1;
      }
    }

  return 0;
}


// if err < 0, not check the matrix
int CheckAndReceiveTransformMessage(igtl::Socket* socket, igtl::MessageHeader* headerMsg,
                                    const char* name, igtl::Matrix4x4& matrix, double err)
{

  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") != 0)
    {
    return 0;
    }

  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(headerMsg);
  transMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    igtl::Matrix4x4 rmatrix;
    transMsg->GetMatrix(rmatrix);
    if (strcmp(transMsg->GetDeviceName(), name) == 0)
      {
      if (err < 0)
        {
        return 1;
        }
      for (int i = 0; i < 4; i ++)
        {
        for (int j = 0; j < 4; j ++)
          {
          if (fabs(rmatrix[i][j] - rmatrix[i][j]) <= err)
            {
            // matrix does not match
            return 0; 
            }
          }
        }
      return 1;
      }
    }
  return 0;
}


int SkipMesage(igtl::Socket* socket, igtl::MessageHeader* headerMsg)
{
  socket->Skip(headerMsg->GetBodySizeToRead(), 0);
  socket->CloseSocket();
  return 1;
}


int SendStringMessage(igtl::Socket* socket, const char* name, const char* string)
{

  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  ts->GetTime();

  stringMsg->SetDeviceName(name);
  stringMsg->SetString(string);
  stringMsg->SetTimeStamp(ts);
  stringMsg->Pack();
  int r = socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize());
  if (!r)
    {
    std::cerr << "ERROR: Seinding STRING( " << name << ", " << string << ")" << std::endl;
    exit(0);
    }
  return 1;
}


int SendTransformMessage(igtl::Socket* socket, const char* name, igtl::Matrix4x4& matrix)
{

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName(name);

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  ts->GetTime();

  transMsg->SetMatrix(matrix);
  transMsg->SetTimeStamp(ts);
  transMsg->Pack();

  int r = socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
  if (!r)
    {
    std::cerr << "ERROR: Seinding TRANSFORM( " << name << ")" << std::endl;
    exit(0);
    }

  return 1;

}


void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

