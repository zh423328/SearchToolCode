#include "stdafx.h"
#include "LuaManager.h"


static LuaManager * m_pShareLuaManager = NULL;

LuaManager* LuaManager::shareLuaManager()
{
	if (m_pShareLuaManager == NULL)
	{
		m_pShareLuaManager = new LuaManager();
	}

	return m_pShareLuaManager;
}

void LuaManager::Purge()
{
	CC_SAFE_DELETE(m_pShareLuaManager);
}

//��ȡluaȫ�ֱ�����ֵ
const char* LuaManager::getLuaVarString(const char* varName )
{
	lua_State*  ls = LuaManager::shareLuaManager()->getLuaState();

	//int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
	//if(isOpen!=0){
	//	CCLOG("Open Lua Error: %i", isOpen);
	//	return NULL;
	//}

	lua_settop(ls, 0);	//ջ�ײ�
	lua_getglobal(ls, varName);		//��ȡ����ջ��

	//�Ƿ���һ���ַ���
	int statesCode = lua_isstring(ls, 1);
	if(statesCode!=1){
		//CCLOG("Open Lua Error: %i", statesCode);
		return NULL;
	}

	const char* str = lua_tostring(ls, 1);	//��ȡֵ

	lua_pop(ls, 1);		//ɾ��global val

	return str;
}

const char* LuaManager::getLuaVarString( const char * szFileName,const char* varName )
{
	std::string fullPath = szFileName;

	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	int nRet = luaL_dofile(ls, fullPath.c_str());
	// lua_gc(m_state, LUA_GCCOLLECT, 0);

	if (nRet != 0)
	{
		//("[LUA ERROR] %s", lua_tostring(ls, -1));
		lua_pop(ls, 1);
		return NULL;
	}
	return getLuaVarString(varName);
}

const char* LuaManager::getLuaVarOfTable(const char* varName,const char* keyName )
{
	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	//int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
	//if(isOpen!=0){
	//	CCLOG("Open Lua Error: %i", isOpen);
	//	return NULL;
	//}

	lua_getglobal(ls, varName);

	int statesCode = lua_istable(ls, -1);
	if(statesCode!=1){
		//CCLOG("Open Lua Error: %i", statesCode);
		return NULL;
	}

	//lua_getfield(ls,-1,keyname)
	lua_pushstring(ls, keyName);
	lua_gettable(ls, -2);
	const char* valueString = lua_tostring(ls, -1);

	lua_pop(ls, -1);

	return valueString;
}

const char* LuaManager::getLuaVarOfTable( const char * szFileName,const char* varName,const char* keyName )
{
	std::string fullPath = szFileName;

	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	int nRet = luaL_dofile(ls, fullPath.c_str());
	// lua_gc(m_state, LUA_GCCOLLECT, 0);

	if (nRet != 0)
	{
		//CCLOG("[LUA ERROR] %s", lua_tostring(ls, -1));
		lua_pop(ls, 1);
		return NULL;
	}
	return getLuaVarOfTable(varName,keyName);
}

//��������table
bool LuaManager::getLuaVarTable(const char* varName,SimpleStringMap&MapValue)
{
	//�������
	//lua_State*  ls = CCLuaEngine::defaultEngine()->getLuaStack()->getLuaState();
	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	//int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
	//if(isOpen!=0){
	//	CCLOG("Open Lua Error: %i", isOpen);
	//	return NULL;
	//}

	//��ȡtable��ֵ
	lua_getglobal(ls, varName);

	if (lua_istable(ls,-1))//ջ��
	{
		int it = lua_gettop(ls);
		lua_pushnil(ls);

		/*��ʱ��ջ״̬
		------
		-2 nil
		-1 table
		------
		*/


		//lua_next ����ѭ������table��
		while(lua_next(ls, it))
		{
			std::string key = "";
			std::string value = "";

			
			/*��ʱ��ջ״̬
			------
			-1 value
			-2 key
			-3 table
			------
			*/

			if (lua_isnumber(ls,-2) == 1)
			{
				int keynum = (int)lua_tonumber(ls,-2);
				char szTmp[20] = {0};
				sprintf(szTmp,"%d",keynum);
				key = szTmp;
			}
			else if (lua_isstring(ls,-2) == 1)
			{
				key = lua_tostring(ls,-2);
			}

			if (lua_isnumber(ls,-1) == 1)
			{
				int keynum = (int)lua_tonumber(ls,-1);
				char szTmp[20] = {0};
				sprintf(szTmp,"%d",keynum);
				value = szTmp;
			}
			else if (lua_isstring(ls,-1) == 1)
			{
				value = lua_tostring(ls,-1);
			}

			MapValue.Push(key,value);

			lua_pop(ls, 1);	//����value,����key��Ϊ��һ��lua_next�Ĳ���
			/*��ʱ��ջ״̬
			------
			-2 key
			-1 table
			------
			*/

		}
		lua_pop(ls, 1);
	}
	else
		return false;

	return true;
}

bool LuaManager::getLuaVarTable( const char* szFileName,const char* varName,SimpleStringMap &MapValue )
{
	std::string fullPath = szFileName;

	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	int nRet = luaL_dofile(ls,fullPath.c_str());//luaL_loadfile(ls, fullPath.c_str()); ֻ�Ǽ������ļ��ˡ�
	// lua_gc(m_state, LUA_GCCOLLECT, 0);

	if (nRet != 0)
	{
		//CCLOG("[LUA ERROR] %s", lua_tostring(ls, -1));
		lua_pop(ls, 1);
		return false;
	}
	return getLuaVarTable(varName,MapValue);
}

//ParamIn��������ParamOut����out
bool LuaManager::callLuaFuncPar(const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut )
{
	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();
    
 /*   int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
        return NULL;
    }
    */
	lua_getglobal(ls, functionName);

	//�����������
	for(int i = 0; i < ParamIn.GetCount(); i++)
	{
		PushLuaData(ls, ParamIn.GetParam(i));
	}

	int nRet = lua_pcall(ls, ParamIn.GetCount(), ParamOut.GetCount(), 0);
	if (nRet != 0)
	{
		//CCLOG("[CLuaFn::CallFileFn]call function(%s) error(%s).\n", functionName, lua_tostring(ls, -1));//����Ļ�����ջ
		return false;
	}

	//���������ջ�����Ƚ����
	int nPos = 0;
	for(int i = ParamOut.GetCount()-1; i >= 0; i--)
	{
		nPos--;			//
		PopLuaData(ls, ParamOut.GetParam(i), nPos);
	}

	int nCount = lua_gettop(ls);

	lua_settop(ls, -1-ParamOut.GetCount());		//��ջ

	return true; 
}

bool LuaManager::callLuaFuncPar( const char* szFileName,const char* functionName,CParamGroup &ParamIn,CParamGroup &ParamOut )
{
	//bool bRet = LoadLuaFile(szFileName);
	std::string fullPath = szFileName;//CCFileUtils::sharedFileUtils()->fullPathForFilename(szFileName);

	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	int nRet = luaL_dofile(ls, fullPath.c_str());
	// lua_gc(m_state, LUA_GCCOLLECT, 0);

	if (nRet != 0)
	{
		//CCLOG("[LUA ERROR] %s", lua_tostring(ls, -1));
		lua_pop(ls, 1);
		return false;
	}
	return callLuaFuncPar(functionName,ParamIn,ParamOut);
}

//���������Ҳ�����ջ
bool LuaManager::PushLuaData( lua_State* pState, _ParamData* pParamIn )
{
	if (pState == NULL || pParamIn == NULL)
		return false;

	if (pParamIn->CompareType("string"))
	{
		lua_pushstring(pState,(char*)pParamIn->GetParam());
		return true;
	}
	else if (pParamIn->CompareType("int"))
	{
		int data = *(int*)pParamIn->GetParam();

		lua_pushinteger(pState,data);
		return true;
	}
	else
	{
		void *pVoid = pParamIn->GetParam();

		//tolua_pushusertype(pState,pVoid,pParamIn->GetType());		//ѹ��ջ�У�

		return true;
	}

	return false;
}

bool LuaManager::PopLuaData( lua_State* pState, _ParamData* pParamOut, int nIndex )
{
	if(pParamOut == NULL)
	{
		return false;
	}

	if(pParamOut->CompareType("string"))
	{
		if (lua_isstring(pState, nIndex) == 1)	//
		{
			const char* pData = (const char*)lua_tostring(pState, nIndex);
			pParamOut->SetData((void* )pData, (int)strlen(pData));
		}
		return true;
	}
	else if(pParamOut->CompareType("int"))
	{
		if (lua_isnumber(pState, nIndex) == 1)
		{
			int nData = (int)lua_tonumber(pState, nIndex);
			pParamOut->SetData(&nData, sizeof(int));
		}
		return true;
	}
	else
	{
		//pParamOut->SetData(tolua_tousertype(pState, nIndex, NULL), -1);
		return true;
	} 
}

//Ԥ�ȼ���luabuf 
bool LuaManager::LoadLuaFile( const char* fileName )
{
	lua_State * pls = LuaManager::shareLuaManager()->getLuaState();

	int nRet = luaL_dofile(pls,fileName);


	return nRet == 0;
}

LuaManager::LuaManager()
{
	m_pls = lua_open();

	//luaopen_base(m_pls);         /* opens the basic library */

	//luaopen_table(m_pls);        /* opens the table library */

	////luaopen_io(m_pls);           /* opens the I/O library */

	//luaopen_string(m_pls);       /* opens the string lib. */

	//luaopen_math(m_pls);         /* opens the math lib. */

	 luaL_openlibs(m_pls);
}

LuaManager::~LuaManager()
{
	 lua_close(m_pls);
}
