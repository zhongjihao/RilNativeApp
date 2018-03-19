/*************************************************************************
   > File Name: ServerException.h
   > Author: zhongjihao
   > Mail: zhongjihao100@163.com 
   > Created Time: Mon 03 Apr 2017 10:05:05 AM CST
 **************************************************************************/

#ifndef SERVER_EXCEPTION_H
#define SERVER_EXCEPTION_H

#include <exception>
//#include <stdarg.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace std;

class ServerException : public exception
{
private:
    char msg[1024];
	int errorCode;
public:
    ServerException();
    ServerException(int errorCode,const char* msg);
    virtual const char* what() const throw();
	int geterrorCode()
	{
		return errorCode;
	}
};

#endif

