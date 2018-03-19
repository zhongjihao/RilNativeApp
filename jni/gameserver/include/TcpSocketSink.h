//
// Created by SPREADTRUM\jihao.zhong on 17-6-16.
//

#ifndef CHATFRAME_TCPSOCKETSINK_H
#define CHATFRAME_TCPSOCKETSINK_H

#include "GameData.h"

class TcpSocketSink
{
public:
	virtual ~TcpSocketSink(){}
    virtual bool OnEventTCPSocketRead(int fd,CMD_Command Command, void* pData, unsigned short wDataSize) = 0;
};

#endif //CHATFRAME_TCPSOCKETSINK_H
