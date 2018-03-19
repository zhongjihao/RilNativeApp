/*************************************************************************
    > File Name: CIniParse.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 22 Jul 2017 02:30:48 PM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CIniParse"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>
#include "../include/CIniParse.h"

using namespace std;

/****************************************************************************** 
 * * 功  能：构造函数 
 * * 参  数：无 
 * * 返回值：无
 * ******************************************************************************/
CIniParse::CIniParse()
{
	memset(m_szKey,0,sizeof(m_szKey));  
	m_fp = NULL;  
}

/****************************************************************************** 
 * * 功  能：析构函数 
 * * 参  数：无 
 * * 返回值：无 
 * ******************************************************************************/  
CIniParse::~CIniParse()
{
	m_Map.clear();
	CloseFile();
}

/****************************************************************************** 
 * * 功  能：删除指定字符串函数 
 * * 参  数：要删除的字符串
 * * 返回值：无 
 * ******************************************************************************/  
void CIniParse::removeAllStr(string& source,const char* str)
{
	unsigned int begin = 0;
	begin = source.find(str,begin);  //查找str第一次出现的位置
	while(begin != string::npos)  //表示字符串中存在该字符串str
	{
		source.replace(begin,1,"");  //用空串替换从begin开始的1个字符
        begin = source.find(str,begin);  //查找str在替换后的source中第一次出现的位置
	}
}

/****************************************************************************** 
 * * 功  能：打开文件函数 
 * * 参  数：无 
 * * 返回值：打开结果码
 * ******************************************************************************/  
INI_RES CIniParse::OpenFile(const char* pathName, const char* type)
{
	string szLine,szMainKey,szLastMainKey,szSubKey;  
    char strLine[CONFIGLEN];
	memset(strLine,0,sizeof(strLine));
    unsigned int  nIndexPos = 0;  
    unsigned int  nLeftPos = 0;  
	unsigned int  nRightPos = 0;

	m_fp = fopen(pathName,type);
    if(m_fp == NULL)
	{
	    ALOGE("%s: ====Game===open inifile %s : %s",__FUNCTION__,pathName,strerror(errno));
        return INI_OPENFILE_ERROR;
	}

	m_Map.clear();
//	mLastMap.clear();  
	while(fgets(strLine,CONFIGLEN,m_fp))
	{
		szLine.assign(strLine);
		//删除字符串中的换行符
		nLeftPos = szLine.find("\n");
		if(string::npos != nLeftPos)
		{
			szLine.erase(nLeftPos,1);
		}
        //删除字符串中的回车符 
		nLeftPos = szLine.find("\r");
        if(string::npos != nLeftPos)
		{
			szLine.erase(nLeftPos,1);  
		}
		//跳过注释行
		if(szLine.find("#") != string::npos)
			continue;

        //判断是否是主键 
		nLeftPos = szLine.find("[");  
		nRightPos = szLine.find("]");
		if(nLeftPos != string::npos && nRightPos != string::npos)
		{
			szLine.erase(nLeftPos,1);
			nRightPos--;  
			szLine.erase(nRightPos,1);
		    //删除字符串中的制表符
		    removeAllStr(szLine,"\t");
            //删除字符串中的空格
			removeAllStr(szLine," ");

			szLastMainKey = szLine;

            KEYMAP mLastMap; 
			mLastMap.clear();  
		//	m_Map[szLastMainKey] = mLastMap;
		    m_Map.insert(make_pair(szLastMainKey,mLastMap));
		}
		//是否是子键
        else if(szLine.length() >0)      
		{
		    //删除字符串中的空格
		    removeAllStr(szLine," ");
		    //删除字符串中的制表符
		    removeAllStr(szLine,"\t");
			nIndexPos = szLine.find("=");

			if(string::npos != nIndexPos)
			{
				string szSubKey,szSubValue;  
                szSubKey = szLine.substr(0,nIndexPos);  
	            szSubValue = szLine.substr(nIndexPos+1,szLine.length()-nIndexPos-1);
				
				typedef multimap<string, map<string,string> >::iterator MainKeyIter;
                MainKeyIter beg = m_Map.upper_bound(szLastMainKey);
			
				map<string,string>& MainValue = (--beg) -> second;
                MainValue[szSubKey] = szSubValue;			
			}
            else
			{
				//不符合ini键值模板的内容 如注释等
			}
		}
	}

	/*
    multimap<string, map<string,string> >::iterator m1_Iter;
	for(m1_Iter = m_Map.begin(); m1_Iter != m_Map.end(); m1_Iter++)
	{
		if(strcmp((m1_Iter->first).c_str(),"tagGameServer")==0)
		{
			map<string,string> data = m1_Iter ->second;
			for(map<string,string>::iterator it = data.begin();it != data.end();it++)
			{
				cout <<  "===原始数据=====" << it->first << " " << it->second << endl;
			}
		}
	}
    */
	return INI_SUCCESS;  
}

/****************************************************************************** 
 * * 功  能：关闭文件函数 
 * * 参  数：无 
 * * 返回值：关闭结果码
 * ******************************************************************************/
INI_RES CIniParse::CloseFile()
{
	if(m_fp != NULL)
	{
		fclose(m_fp);  
		m_fp = NULL;
	}
	
	return INI_SUCCESS;
}

/****************************************************************************** 
 * * 功  能：获取[SECTION]下的某一个键值的字符串 
 * * 参  数： 
 * *  char* mAttr  输入参数 主键 
 * *  char* cAttr  输入参数 子键 
 * *  char* value  输出参数 子键键值 
 * * 返回值：结果码
 * ******************************************************************************/ 
INI_RES CIniParse::GetKey(unsigned int index,const char* mAttr, const char* cAttr, char* pValue)
{
	typedef multimap<string, map<string,string> >::const_iterator MainKeyIter;
    MainKeyIter iter = m_Map.find(mAttr);
	for(unsigned int i = 0; i != m_Map.count(mAttr); i++,iter++)
	{
		if(i == index)
		{
			KEYMAP mKey = iter ->second;
			if(mKey.size()>0)
			{
				string sTemp = mKey[cAttr];
		        memcpy(pValue,sTemp.c_str(),strlen(sTemp.c_str()));
		        return INI_SUCCESS;
			}
			else
				return INI_NO_ATTR;
		}
	}
    return INI_NO_ATTR;
}

/****************************************************************************** 
 * * 功  能：获取键值对应的整形 
 * * 参  数： 
 * *   cAttr                     主键 
 * *   cAttr                     子键 
 * *   buff                      存放结果
 * * 返回值：正常则返回对应的数值 未读取成功则返回0(键值本身为0不冲突) 
 * ******************************************************************************/
int CIniParse::GetInt(unsigned int index,const char* mAttr,const char* cAttr)
{
	memset(m_szKey,0,sizeof(m_szKey));
	int nRes = 0;
	if(INI_SUCCESS == GetKey(index,mAttr,cAttr,m_szKey))  
	{
		nRes = atoi(m_szKey);
	}
    return nRes;
}

/****************************************************************************** 
 * * 功  能：获取键值对应的字符串 
 * * 参  数： 
 * *    cAttr                     主键 
 * *    cAttr                     子键 
 * *    buff                      存放结果
 * * 返回值：正常则返回读取到的子键字符串 未读取成功则返回"NULL"
 * ******************************************************************************/
char* CIniParse::GetStr(unsigned int index,const char* mAttr,const char* cAttr)
{
	memset(m_szKey,0,sizeof(m_szKey));
	if(INI_SUCCESS != GetKey(index,mAttr,cAttr,m_szKey))
	{
		strcpy(m_szKey,"NULL");
	}
	return m_szKey;
}

int CIniParse::CountKeyElement(const char* key)const
{
	return m_Map.count(key);
}

