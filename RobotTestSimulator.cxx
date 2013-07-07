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
#include <string>
#include <vector>

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

#include "RobotSimulatorPhaseBase.h"
#include "RobotSimulatorUndefinedPhase.h"
#include "RobotSimulatorStartUpPhase.h"
#include "RobotSimulatorPlanningPhase.h"
#include "RobotSimulatorCalibrationPhase.h"
#include "RobotSimulatorTargetingPhase.h"
#include "RobotSimulatorMoveToTargetPhase.h"
#include "RobotSimulatorManualPhase.h"
#include "RobotSimulatorStopPhase.h"
#include "RobotSimulatorEmergencyPhase.h"

int Session(igtl::Socket * socket);

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
      std::cerr << "Client connected. Starting a session..." << std::endl;
      Session(socket);
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  
  socket->CloseSocket();

}


int Session(igtl::Socket * socket)
{
  std::vector< RobotSimulatorPhaseBase* > workphase;

  workphase.push_back(new RobotSimulatorUndefinedPhase);
  workphase.push_back(new RobotSimulatorStartUpPhase);
  workphase.push_back(new RobotSimulatorPlanningPhase);
  workphase.push_back(new RobotSimulatorCalibrationPhase);
  workphase.push_back(new RobotSimulatorTargetingPhase);
  workphase.push_back(new RobotSimulatorMoveToTargetPhase);
  workphase.push_back(new RobotSimulatorManualPhase);
  workphase.push_back(new RobotSimulatorStopPhase);
  workphase.push_back(new RobotSimulatorEmergencyPhase);

  //------------------------------------------------------------
  // Set socket.
  std::vector< RobotSimulatorPhaseBase* >::iterator iter;
  for (iter = workphase.begin(); iter != workphase.end(); iter ++)
    {
    std::cerr << "MESSAGE: Setting up " << (*iter)->Name() << " phase." << std::endl;
    (*iter)->SetSocket(socket);
    }

  //------------------------------------------------------------
  // Set undefined phase as the current phase;
  std::vector<  RobotSimulatorPhaseBase* >::iterator currentPhase = workphase.begin();

  int connect = 1;

  //------------------------------------------------------------
  // loop
  while (connect)
    {
    if ((*currentPhase)->Process())
      {
      // If Process() returns 1, phase change has been requested.
      std::string requestedWorkphase = (*currentPhase)->GetNextWorkPhase();
      std::string queryID = (*currentPhase)->GetQueryID();
      
      // Find the requested workphase
      std::vector<  RobotSimulatorPhaseBase* >::iterator iter;
      for (iter = workphase.begin(); iter != workphase.end(); iter ++)
        {
        if (strcmp((*iter)->Name(), requestedWorkphase.c_str()) == 0)
          {
          // Change the current phase
          currentPhase = iter;
          (*currentPhase)->Enter(queryID.c_str()); // Initialization process
          }
        }
      }
    }
  
  return 1;
}


