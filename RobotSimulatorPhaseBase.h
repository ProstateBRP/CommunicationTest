/*=========================================================================

  Program:   BRP Prostate Robot: Testing Simulator (Robot)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  Please see
    http://wiki.na-mic.org/Wiki/index.php/ProstateBRP_OpenIGTLink_Communication_June_2013
  for the detail of the testing protocol.

=========================================================================*/

#ifndef __RobotSimulatorPhaseBase_h
#define __RobotSimulatorPhaseBase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "TestBase.h"

class RobotSimulatorPhaseBase : public TestBase
{
public:

  RobotSimulatorPhaseBase();
  ~RobotSimulatorPhaseBase();

  virtual const char* Name() { return "NONE"; };

  // Process() will be called by the main session loop.
  // It checks if any work phase change request is recevied first. If not it calles
  // MessageHander() to perform workphase-specific message handling. 
  virtual const char* Process();

  // MessageHandler() defines workphase-specific message handling.
  // The function needs to be implemented in child classes.
  virtual int MessageHandler(igtl::MessageHeader* headerMsg) = 0; // Message handler

protected:

  // Check if a CMD message (workphase change) has been received.
  // Return 1, if workphase change has been requested.
  int CheckWorkphaseChange(igtl::MessageHeader* headerMsg);

  // Check if there is any messages that must be accepted 
  // regardless of current workhpase.
  int CheckCommonMessage(igtl::MessageHeader* headerMsg);

  igtl::Socket::Pointer Socket;
  std::string NextWorkphase;
};

#endif //__RobotSimulatorPhaseBase_h
