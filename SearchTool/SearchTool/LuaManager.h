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

#define MAX_PARAM			50				//���������



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

//ģ��򵥵�map����
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

	//�ͷ��ڴ�
	void	Purge();
    
	//�����ļ�
	bool	LoadLuaFile(const char* fileName);

    /*
     ֱ�ӻ�ȡLua�еñ�����ֵ
     �ļ��� luaFileName
     ������ varName
     */
	const char* getLuaVarString(const char * szFileName,const char* varName);
    const char* getLuaVarString(const char* varName);
    
    /*
     ��ȡLua��һ�����еýڵ�������������
     �ļ���luaFileName
     ������varName
     �ڵ���
     */
	const char*	getLuaVarOfTable(const char * szFileName,const char* varName,const char* keyName);		//Ϊ���ȸ����õġ�
    const char* getLuaVarOfTable(const char* varName,const char* keyName);
    
    /*
     ����Luaȫ��Table
     lua�ļ���luaFileName
     table������varName
     */
	bool getLuaVarTable(const char* szFileName,const char* varName,SimpleStringMap &MapValue);
    bool getLuaVarTable(const char* varName,SimpleStringMap &MapValue);
    
    /*
     �������з���
     �ļ���luaFileName
     ������functionName
     ��������ParamIn�������
     ��������ParamOut ���ؽ��
     */
	bool callLuaFuncPar(const char* szFileName,const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut);
    bool callLuaFuncPar(const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut);


	  /*
     in������ջ
     luaջ pState
     ��������ParamIn�������
     */
	bool PushLuaData(lua_State* pState, _ParamData* pParamIn);			//ջ

	 /*
     out������ջ
     luaջ pState
     ��������ParamOut��������
	 nIndex�ڼ�������
     */
	bool PopLuaData(lua_State* pState, _ParamData* pParamOut, int nIndex); 

	//��ȡ���е�luastate
	lua_State * getLuaState()		{ return m_pls;}
private:
	lua_State*  m_pls;
};

#endif