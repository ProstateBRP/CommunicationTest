/*=========================================================================

  Program:   BRP Prostate Robot: Testing Simulator (Server)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  Please see
    http://wiki.na-mic.org/Wiki/index.php/ProstateBRP_OpenIGTLink_Communication_June_2013
  for the detail of the testing protocol.

=========================================================================*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"
#include "igtlPositionMessage.h"
#include "igtlPointMessage.h"
#include "igtlStringMessage.h"
#include "igtlBindMessage.h"

int ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader * header);
int ReceivePosition(igtl::Socket * socket, igtl::MessageHeader * header);
int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header);
int ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
    }

  igtl::Socket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      Session(socket);
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  
  socket->CloseSocket();

}


int Session(igtl::Socekt * socket)
{

  //------------------------------------------------------------
  // loop
  for (;;)
    {
    // Check data type and receive data body
    if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
      {
      std::string str;
      ReceiveString(socket, headerMsg, str);
      }
    if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
      {
      ReceiveTransform(socket, headerMsg);
      }
    else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
      {
      ReceiveStatus(socket, headerMsg);
      }
    else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
      {
      ReceiveString(socket, headerMsg);
      }
    else
      {
      // if the data type is unknown, skip reading.
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      std::cerr << "Size : " << headerMsg->GetBodySizeToRead() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    }
}


const char* CheckPhaseTransition(igtl::Socket * socket, igtl::MessageHeader * header)
{
  
    {
    
}

int ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader * header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    igtl::PrintMatrix(matrix);
    return 1;
    }

  return 0;

}



int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  return 0;

}


 int ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header, std::string& str)
{

  std::cerr << "Receiving STRING data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetMessageHeader(header);
  stringMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = stringMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    str = stringMsg->GetString();
    //std::cerr << "Encoding: " << stringMsg->GetEncoding() << "; "
    //          << "String: " << stringMsg->GetString() << std::endl;
    }

  return 1;
}


