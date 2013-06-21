/*=========================================================================

  Program:   BRP Prostate Robot: Testing Simulator (Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ClientNormalOperationTest_h
#define __ClientNormalOperationTest_h

#include "igtlSocket.h"
#include "ClientTestBase.h"

class ClientNormalOperationTest : public ClientTestBase
{
public:
  ClientNormalOperationTest();
  ~ClientNormalOperationTest();

  virtual int Test();

};

#endif //__ClientNormalOperationTest_h

