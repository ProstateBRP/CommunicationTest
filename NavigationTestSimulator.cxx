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

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlClientSocket.h"
#include "NavigationNormalOperationTest.h"
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
      NavigationNormalOperationTest * cnotest = new NavigationNormalOperationTest();
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

