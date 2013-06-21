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

#ifndef __ClientTestBase_h
#define __ClientTestBase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

class ClientTestBase
{
public:
  ClientTestBase();
  ~ClientTestBase();

  virtual int Exec();
  virtual int Test() = 0;  // Testing protocol implementation. This must be implemented in a child class.

  void SetSocket(igtl::Socket* socket);
  int  IsReady();

  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
  int  ReceiveMessageHeader(igtl::MessageHeader* headerMsg, int timeout);
  int  SkipMesage(igtl::MessageHeader* headerMsg);
  int  SendStringMessage(const char* name, const char* string);
  int  SendTransformMessage(const char* name, igtl::Matrix4x4& matrix);
  int  CheckAndReceiveStringMessage(igtl::MessageHeader* headerMsg,
                                    const char* name, const char* string);
  int  CheckAndReceiveStatusMessage(igtl::MessageHeader* headerMsg,
                                    const char* name, int code);
  int  CheckAndReceiveTransformMessage(igtl::MessageHeader* headerMsg,
                                       const char* name, igtl::Matrix4x4& matrix, double err = 1.0e-10);

protected:

  igtl::Socket::Pointer Socket;
};

#endif //__ClientTestBase_h
