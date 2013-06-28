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

#include "RobotSimulatorPhaseBase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

RobotSimulatorPhaseBase::RobotSimulatorPhaseBase()
{
}


RobotSimulatorPhaseBase::~RobotSimulatorPhaseBase()
{
}

const char* RobotSimulatorPhaseBase::Process()
{
  // Create a message buffer to receive header
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  //------------------------------------------------------------
  // Allocate a time stamp 
  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  
  // Initialize receive buffer
  headerMsg->InitPack();
  
  // Receive generic header from the socket
  int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
  if (r == 0)
    {
    socket->CloseSocket();
    }
  if (r != headerMsg->GetPackSize())
    {
    return NULL;
    }
  
  // Deserialize the header
  headerMsg->Unpack();
  
  //// Get time stamp
  //igtlUint32 sec;
  //igtlUint32 nanosec;
  
  //headerMsg->GetTimeStamp(ts);
  //ts->GetTimeStamp(&sec, &nanosec);
  //
  //std::cerr << "Time stamp: "
  //          << sec << "." << std::setw(9) << std::setfill('0') 
  //          << nanosec << std::endl;
  
  // Workphase change.

}

int RobotSimulatorPhaseBase::MessageHandler(igtl::MessageHeader* headerMsg)
{
}


