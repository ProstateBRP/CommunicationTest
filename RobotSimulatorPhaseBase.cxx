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
  this->NextWorkphase.clear();
}


RobotSimulatorPhaseBase::~RobotSimulatorPhaseBase()
{
}


int RobotSimulatorPhaseBase::Enter(const char* queryID)
{
  // Send acknowledgement message with query ID
  std::stringstream ss;
  ss << "ACK_" << queryID << std::endl;
  this->SendStringMessage(ss.str().c_str(), this->Name());
  return this->Initialize();
}


int RobotSimulatorPhaseBase::Process()
{
  // Create a message buffer to receive header
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  ReceiveMessageHeader(headerMsg, 0);

  // If there is any workphase change request,
  // set NextWorkphase (done in the subroutine) and return 1.
  if (this->CheckWorkphaseChange(headerMsg))
    {
    return 1;
    }

  // Otherwise, the current workphase is the next workphase.
  this->NextWorkphase = this->Name(); // Set the name of the current workphase as the next one.
  if (!this->CheckCommonMessage(headerMsg))
    {
    this->MessageHandler(headerMsg);
    }

  return 0;

}


int RobotSimulatorPhaseBase::CheckWorkphaseChange(igtl::MessageHeader* headerMsg)
{

  // Check if the message requests phase transition
  if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0 &&
      strncmp(headerMsg->GetDeviceName(), "CMD_", 4) == 0)
    {
    igtl::StringMessage::Pointer stringMsg;
    stringMsg = igtl::StringMessage::New();
    stringMsg->SetMessageHeader(headerMsg);
    stringMsg->AllocatePack();
    this->Socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());
    
    // Deserialize the string message
    // If you want to skip CRC check, call Unpack() without argument.
    int c = stringMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      if (stringMsg->GetEncoding() == 3)
        {
        this->NextWorkphase = stringMsg->GetString();
        // Get the query ID
        std::string msgName = headerMsg->GetDeviceName();
        this->QueryID = msgName.substr(4, std::string::npos);

        return 1;
        }
      else
        {
        this->NextWorkphase = "Unknown";
        return 1;
        }
      }
    else
      {
      std::cerr << "ERROR: Invalid CRC." << std::endl;
      this->NextWorkphase = "Unknown";
      return 1;
      }
    }
  else
    {
    return 0;
    }
}


int RobotSimulatorPhaseBase::CheckCommonMessage(igtl::MessageHeader* headerMsg)
{
  /// Check if GET_TRANSFORM has been received
  if (strcmp(headerMsg->GetDeviceType(), "GET_TRANSFORM") == 0 &&
      strncmp(headerMsg->GetDeviceName(), "CURRENT_POSITION", 4) == 0)
    {
    return 1;
    }
  /// Check if GET_STATUS has been received
  else if (strcmp(headerMsg->GetDeviceType(), "GET_STATUS") == 0 &&
           strncmp(headerMsg->GetDeviceName(), "CURRENT_STATUS", 4) == 0)
    {
    this->SendStatusMessage(this->Name(), 1, 0);
    return 1;
    }

  return 0;
}


