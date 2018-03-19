/*************************************************************************
    > File Name: CIniParse.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 22 Jul 2017 02:01:45 PM CST
 ************************************************************************/

#ifndef _CINI_PARSE_H_
#define _CINI_PARSE_H_

#include <map>
#include <string> 

using namespace std;

#define CONFIGLEN           256

enum INI_RES
{
	INI_SUCCESS,             //成功
	INI_ERROR,               //普通错误
	INI_OPENFILE_ERROR,      //打开文件失败
	INI_NO_ATTR              //无对应的键值
};

//              子键索引    子键值
typedef map<std::string,std::string> KEYMAP;
//              主键索引 主键值
typedef multimap<std::string,KEYMAP> MAINKEYMAP;

//config文件的基本操作类
class CIniParse
{
protected:
	//被打开的文件局柄 
	FILE* m_fp;  
	char  m_szKey[CONFIGLEN];  
	MAINKEYMAP m_Map; 

public:
	CIniParse();
	virtual ~CIniParse();

public:
	//获取整形的键值
	int GetInt(unsigned int index,const char* mAttr, const char* cAttr);
	//获取键值的字符串
	char* GetStr(unsigned int index,const char* mAttr, const char* cAttr);
	//打开config 文件
	INI_RES OpenFile(const char* pathName, const char* type);
	//关闭config 文件
	INI_RES CloseFile();
	int CountKeyElement(const char* key)const;

protected:
	//读取config文件
	INI_RES GetKey(unsigned int index,const char* mAttr, const char* cAttr, char* value);
	//删除字符串中指定的所有字符串str
	void removeAllStr(string & source,const char* str);
};

#endif

