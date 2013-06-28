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

#ifndef __NavigationTestBase_h
#define __NavigationTestBase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "TestBase.h"

class NavigationTestBase : public TestBase
{
public:
  enum {
    ERROR = 0,
    SUCCESS = 1,
  };

public:
  NavigationTestBase();
  ~NavigationTestBase();

  virtual const char* Name()=0;

  virtual int Exec();
  virtual int Test() = 0;  // Testing protocol implementation. This must be implemented in a child class.

protected:

  igtl::Socket::Pointer Socket;
};

#endif //__NavigationTestBase_h
