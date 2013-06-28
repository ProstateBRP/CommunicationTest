/*=========================================================================

  Program:  BRP Prostate Robot: Testing Simulator (Robot)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  Please see
    http://wiki.na-mic.org/Wiki/index.php/ProstateBRP_OpenIGTLink_Communication_June_2013
  for the detail of the testing protocol.

=========================================================================*/

#ifndef __CommunicationTestBase_h
#define __CommunicationTestBase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

class CommunicationTestBase
{
public:
  enum {
    ERROR = 0,
    SUCCESS = 1,
  };

public:
  CommunicationTestBase();
  ~CommunicationTestBase();

  virtual const char* Name()=0;

  void SetSocket(igtl::Socket* socket);
  int  ReceiveMessageHeader(igtl::MessageHeader* headerMsg, int timeout);
  int  SkipMesage(igtl::MessageHeader* headerMsg);
  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
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

#endif //__CommunicationTestBase_h
