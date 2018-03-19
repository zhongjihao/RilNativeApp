/*************************************************************************
    > File Name: ServerException.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 10:08:12 AM CST
 *************************************************************************/


#include "../include/ServerException.h"

ServerException::ServerException()
{
    bzero(msg,sizeof(msg));
    sprintf(msg,"RIL Native异常!\n");
}

ServerException::ServerException(int error,const char* msg)
{
    bzero(this->msg,sizeof(this->msg));
    errorCode = error;
	sprintf(this->msg,"%s",msg);
	//va_list args; //定义va_list
    //va_start(args, fmt);
	//vsnprintf(this->msg,sizeof(this->msg),(char *)fmt,args);
    //va_end(args);//结束获取
}

const char* ServerException::what()const throw()
{
    return msg;
}

