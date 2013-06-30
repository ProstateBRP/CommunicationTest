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

#include "TestBase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

TestBase::TestBase()
{
}


TestBase::~TestBase()
{
}


void TestBase::SetSocket(igtl::Socket* socket)
{
  this->Socket = socket;
}


int TestBase::ReceiveMessageHeader(igtl::MessageHeader* headerMsg, int timeout)
{

  this->Socket->SetTimeout(timeout);

  headerMsg->InitPack();
  int r = this->Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
  if (r != headerMsg->GetPackSize())
    {
    if (r < 0) // Timeout
      {
      std::cerr << "ERROR: Timeout." << std::endl;
      this->Socket->CloseSocket();
      exit(0);
      }
    else
      {
      std::cerr << "ERROR: Receiving message." << std::endl;
      this->Socket->CloseSocket();
      exit(0);
      }
    }
  headerMsg->Unpack();
  return 1;
}


int TestBase::CheckAndReceiveStringMessage(igtl::MessageHeader* headerMsg,
                                           const char* name, const char* string, int suffix)
{

  int success = 0;

  if (strcmp(headerMsg->GetDeviceType(), "STRING") != 0)
    {
    std::cerr << "ERROR: Wrong message type." << std::endl;
    success = 0;
    }
  else if (suffix ?
           strncmp(name, headerMsg->GetDeviceName(), strlen(name)) != 0 :
           strcmp(name, headerMsg->GetDeviceName()) != 0)
    {
    std::cerr << "ERROR: Wrong message name." << std::endl;
    success = 0;
    }
  else
    {
    // Create a message buffer to receive transform data
    igtl::StringMessage::Pointer stringMsg;
    stringMsg = igtl::StringMessage::New();
    stringMsg->SetMessageHeader(headerMsg);
    stringMsg->AllocatePack();
    
    // Receive string data from the socket
    this->Socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());
    
    // Deserialize the transform data
    // If you want to skip CRC check, call Unpack() without argument.
    int c = stringMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      if (stringMsg->GetEncoding() == 3 &&
          strcmp(stringMsg->GetString(), string) == 0)
        {
        success = 1;
        }
      else
        {
        std::cerr << "ERROR: Invalid string: Encoding=" << stringMsg->GetEncoding()
                  << ", String=" << string << std::endl;
        success = 0;
        }
      }
    else
      {
      std::cerr << "ERROR: Invalid CRC." << std::endl;
      success = 0;
      }
    }
  
  if (!success)
    {
    std::cerr << "ERROR: Could not receive STRING(" << name << ", " << string << ") message." << std::endl;
    this->Socket->CloseSocket();
    }

  return success;

}


int TestBase::CheckAndReceiveStatusMessage(igtl::MessageHeader* headerMsg,
                                           const char* name, int code, int suffix)
{
  
  int success = 0;
  
  if (strcmp(headerMsg->GetDeviceType(), "STATUS") != 0)
    {
    std::cerr << "ERROR: Wrong message type." << std::endl;
    success = 0;
    }
  else if (suffix ?
           strncmp(name, headerMsg->GetDeviceName(), strlen(name)) != 0 :
           strcmp(name, headerMsg->GetDeviceName()) != 0)
    {
    std::cerr << "ERROR: Wrong message name." << std::endl;
    success = 0;
    }
  else
    {
    // Create a message buffer to receive transform data
    igtl::StatusMessage::Pointer statusMsg;
    statusMsg = igtl::StatusMessage::New();
    statusMsg->SetMessageHeader(headerMsg);
    statusMsg->AllocatePack();
    
    // Receive status data from the socket
    this->Socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
    
    // Deserialize the transform data
    // If you want to skip CRC check, call Unpack() without argument.
    int c = statusMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      if (statusMsg->GetCode() == code)
        {
        success = 1;
        }
      else
        {
        std::cerr << "ERROR: Invalid Code: " << code << std::endl;
        success = 0;
        }
      }
    else
      {
      std::cerr << "ERROR: Invalid CRC." << std::endl;
      success = 0;
      }
    }

  if (!success)
    {
    std::cerr << "ERROR: Could not receive STATUS(" << name << ", " << code << ") message." << std::endl;
    this->Socket->CloseSocket();
    }

  return success;
}


// if err < 0, not check the matrix
int TestBase::CheckAndReceiveTransformMessage(igtl::MessageHeader* headerMsg,
                                              const char* name, igtl::Matrix4x4& matrix, double err, 
                                              int suffix)
{
  int success = 0;

  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") != 0)
    {
    std::cerr << "ERROR: Wrong message type." << std::endl;
    success = 0;
    }
  else if (suffix ?
           strncmp(name, headerMsg->GetDeviceName(), strlen(name)) != 0 :
           strcmp(name, headerMsg->GetDeviceName()) != 0)
    {
    std::cerr << "ERROR: Wrong message name." << std::endl;
    success = 0;
    }
  else
    {
    // Create a message buffer to receive transform data
    igtl::TransformMessage::Pointer transMsg;
    transMsg = igtl::TransformMessage::New();
    transMsg->SetMessageHeader(headerMsg);
    transMsg->AllocatePack();
    
    // Receive transform data from the socket
    this->Socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
    
    // Deserialize the transform data
    // If you want to skip CRC check, call Unpack() without argument.
    int c = transMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      igtl::Matrix4x4 rmatrix;
      transMsg->GetMatrix(rmatrix);
      if (err < 0) // Ignore error check
        {
        success = 1;
        }
      for (int i = 0; i < 4; i ++)
        {
        for (int j = 0; j < 4; j ++)
          {
          if (fabs(rmatrix[i][j] - rmatrix[i][j]) <= err)
            {
            std::cerr << "ERROR: Matrix does not match." << std::endl;
            std::cerr << "==== Expected Matrix ====" << std::endl;
            igtl::PrintMatrix(matrix);
            std::cerr << "==== Received Matrix ====" << std::endl;
            igtl::PrintMatrix(rmatrix);
            success = 0;
            }
          }
        }
      success = 1;
      }
    else
      {
      std::cerr << "ERROR: Invalid CRC." << std::endl;
      success = 0;
      }
    }

  if (!success)
    {
    std::cerr << "ERROR: Could not receive TRANSFORM(" << name << ") message" << std::endl;
    this->Socket->CloseSocket();
    }

  return success;
}


int TestBase::SkipMesage(igtl::MessageHeader* headerMsg)
{
  this->Socket->Skip(headerMsg->GetBodySizeToRead(), 0);
  this->Socket->CloseSocket();
  return 1;
}


int TestBase::SendStringMessage(const char* name, const char* string)
{

  std::cerr << "-- Seinding STRING( " << name << ", " << string << ")" << std::endl;
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  ts->GetTime();

  stringMsg->SetDeviceName(name);
  stringMsg->SetString(string);
  stringMsg->SetTimeStamp(ts);
  stringMsg->Pack();
  int r = this->Socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize());
  if (!r)
    {
    std::cerr << "ERROR: Seinding STRING( " << name << ", " << string << ")" << std::endl;
    exit(0);
    }
  return 1;
}


int TestBase::SendTransformMessage(const char* name, igtl::Matrix4x4& matrix)
{
  std::cerr << "-- Seinding TRANSFORM( " << name << ")" << std::endl;

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName(name);

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  ts->GetTime();

  transMsg->SetMatrix(matrix);
  transMsg->SetTimeStamp(ts);
  transMsg->Pack();

  int r = this->Socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
  if (!r)
    {
    std::cerr << "ERROR: Seinding TRANSFORM( " << name << ")" << std::endl;
    exit(0);
    }

  return 1;

}


int TestBase::SendStatusMessage(const char* name, int Code, int SubCode)
{
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetDeviceName("Device");

  statusMsg->SetCode(Code);
  statusMsg->SetSubCode(SubCode);
  statusMsg->SetErrorName("");
  statusMsg->SetStatusString("");
  statusMsg->Pack();
  int r = this->Socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());
  if (!r)
    {
    std::cerr << "ERROR: Seinding STATUS( " << name << ")" << std::endl;
    exit(0);
    }

  return 1;
}


void TestBase::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
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



