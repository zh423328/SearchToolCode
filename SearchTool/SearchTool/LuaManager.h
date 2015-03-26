#ifndef LUA_MANAGER_H_
#define LUA_MANAGER_H_

#include "stdafx.h"

#include <vector>
#include <map>


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#define MAX_PARAM			50				//最大类型名



struct _ParamData
{
public:
	void* m_pParam;
	char  m_szType[MAX_PARAM];
	int   m_TypeLen;

public:
	_ParamData()
	{
		m_pParam    = NULL;
		m_szType[0] = '\0';
		m_TypeLen   = 0;
	};

	_ParamData(void* pParam, const char* szType, int nTypeLen)
	{
		SetParam(pParam, szType, nTypeLen);
	}

	~_ParamData() {};

	void SetParam(void* pParam, const char* szType, int nTypeLen)
	{
		m_pParam = pParam;
		sprintf(m_szType, "%s", szType);
		m_TypeLen = nTypeLen;
	};

	bool SetData(void* pParam, int nLen)
	{
		if(m_TypeLen < nLen)
		{
			return false;
		}

		if(nLen > 0)
		{
			memcpy(m_pParam, pParam, nLen);
		}
		else
		{
			memcpy(m_pParam, pParam, m_TypeLen);
		}
		return true;
	}

	void* GetParam()
	{
		return m_pParam;
	}

	const char* GetType()
	{
		return m_szType;
	}

	bool CompareType(const char* pType)
	{
		if(0 == strcmp(m_szType, pType))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


class CParamGroup
{
public:
	CParamGroup() {};
	~CParamGroup()
	{
		Close();
	};

	void Init()
	{
		m_vecParamData.clear();
	};

	void Close()
	{
		for(int i = 0; i < (int)m_vecParamData.size(); i++)
		{
			_ParamData* pParamData = m_vecParamData[i];
			CC_SAFE_DELETE(pParamData);
		}
		m_vecParamData.clear();
	};

	void Push(_ParamData* pParam)
	{
		if(pParam != NULL)
		{
			m_vecParamData.push_back(pParam);
		}
	};

	_ParamData* GetParam(int nIndex)
	{
		if(nIndex < (int)m_vecParamData.size() && nIndex >= 0)
		{
			return m_vecParamData[nIndex];
		}
		else
		{
			return NULL;
		}
	};

	int GetCount()
	{
		return (int)m_vecParamData.size();
	}

private:
	typedef std::vector<_ParamData*> vecParamData;
	vecParamData m_vecParamData;
};

//模拟简单的map功能
struct SimpleMap
{
	std::string key;
	std::string value;
};

class SimpleStringMap
{
public:
	SimpleStringMap(){}
	~SimpleStringMap()
	{
		for (int i = 0; i < StringMap.size();++i)
		{
			SimpleMap *pMap = StringMap[i];
			CC_SAFE_DELETE(pMap);
		}
		StringMap.clear();
	}

	void Push(std::string key,std::string value)
	{
		SimpleMap *pMap = new SimpleMap();
		pMap->key = key;
		pMap->value = value;

		StringMap.push_back(pMap);
	}

	std::string GetKey(std::string key)
	{
		for (int i = 0; i < StringMap.size(); ++i)
		{
			SimpleMap *pMap = StringMap[i];
			if (pMap && pMap->key.compare(key) == 0)
				return pMap->value;
		}

		return "";
	}
	std::string operator[](std::string key)
	{
		return GetKey(key);
	}
private:
	typedef std::vector<SimpleMap*> VecMap;

	VecMap StringMap;
};

class LuaManager{
public:
	LuaManager();
	~LuaManager();


    static LuaManager* shareLuaManager();

	//释放内存
	void	Purge();
    
	//加载文件
	bool	LoadLuaFile(const char* fileName);

    /*
     直接获取Lua中得变量名值
     文件名 luaFileName
     变量名 varName
     */
	const char* getLuaVarString(const char * szFileName,const char* varName);
    const char* getLuaVarString(const char* varName);
    
    /*
     获取Lua中一个表中得节点名，甚至方法
     文件名luaFileName
     方法名varName
     节点名
     */
	const char*	getLuaVarOfTable(const char * szFileName,const char* varName,const char* keyName);		//为了热更新用的。
    const char* getLuaVarOfTable(const char* varName,const char* keyName);
    
    /*
     调用Lua全局Table
     lua文件名luaFileName
     table变量名varName
     */
	bool getLuaVarTable(const char* szFileName,const char* varName,SimpleStringMap &MapValue);
    bool getLuaVarTable(const char* varName,SimpleStringMap &MapValue);
    
    /*
     带参数有返回
     文件名luaFileName
     方法名functionName
     参数序列ParamIn传入参数
     参数类型ParamOut 返回结果
     */
	bool callLuaFuncPar(const char* szFileName,const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut);
    bool callLuaFuncPar(const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut);


	  /*
     in参数进栈
     lua栈 pState
     参数序列ParamIn传入参数
     */
	bool PushLuaData(lua_State* pState, _ParamData* pParamIn);			//栈

	 /*
     out参数进栈
     lua栈 pState
     参数序列ParamOut传出参数
	 nIndex第几个参数
     */
	bool PopLuaData(lua_State* pState, _ParamData* pParamOut, int nIndex); 

	//获取运行的luastate
	lua_State * getLuaState()		{ return m_pls;}
private:
	lua_State*  m_pls;
};

#endif