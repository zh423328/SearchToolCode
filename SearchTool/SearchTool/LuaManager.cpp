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

//获取lua全局变量的值
const char* LuaManager::getLuaVarString(const char* varName )
{
	lua_State*  ls = LuaManager::shareLuaManager()->getLuaState();

	//int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
	//if(isOpen!=0){
	//	CCLOG("Open Lua Error: %i", isOpen);
	//	return NULL;
	//}

	lua_settop(ls, 0);	//栈底部
	lua_getglobal(ls, varName);		//获取插入栈中

	//是否是一个字符串
	int statesCode = lua_isstring(ls, 1);
	if(statesCode!=1){
		//CCLOG("Open Lua Error: %i", statesCode);
		return NULL;
	}

	const char* str = lua_tostring(ls, 1);	//获取值

	lua_pop(ls, 1);		//删除global val

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

//返回整个table
bool LuaManager::getLuaVarTable(const char* varName,SimpleStringMap&MapValue)
{
	//参数结果
	//lua_State*  ls = CCLuaEngine::defaultEngine()->getLuaStack()->getLuaState();
	lua_State*  ls =  LuaManager::shareLuaManager()->getLuaState();

	//int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
	//if(isOpen!=0){
	//	CCLOG("Open Lua Error: %i", isOpen);
	//	return NULL;
	//}

	//获取table的值
	lua_getglobal(ls, varName);

	if (lua_istable(ls,-1))//栈顶
	{
		int it = lua_gettop(ls);
		lua_pushnil(ls);

		/*此时堆栈状态
		------
		-2 nil
		-1 table
		------
		*/


		//lua_next 类似循环便利table表
		while(lua_next(ls, it))
		{
			std::string key = "";
			std::string value = "";

			
			/*此时堆栈状态
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

			lua_pop(ls, 1);	//弹出value,留下key作为下一此lua_next的参数
			/*此时堆栈状态
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

	int nRet = luaL_dofile(ls,fullPath.c_str());//luaL_loadfile(ls, fullPath.c_str()); 只是加载了文件了。
	// lua_gc(m_state, LUA_GCCOLLECT, 0);

	if (nRet != 0)
	{
		//CCLOG("[LUA ERROR] %s", lua_tostring(ls, -1));
		lua_pop(ls, 1);
		return false;
	}
	return getLuaVarTable(varName,MapValue);
}

//ParamIn参数进，ParamOut参数out
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

	//加载输入参数
	for(int i = 0; i < ParamIn.GetCount(); i++)
	{
		PushLuaData(ls, ParamIn.GetParam(i));
	}

	int nRet = lua_pcall(ls, ParamIn.GetCount(), ParamOut.GetCount(), 0);
	if (nRet != 0)
	{
		//CCLOG("[CLuaFn::CallFileFn]call function(%s) error(%s).\n", functionName, lua_tostring(ls, -1));//错误的话，进栈
		return false;
	}

	//后面参数在栈顶，先进后出
	int nPos = 0;
	for(int i = ParamOut.GetCount()-1; i >= 0; i--)
	{
		nPos--;			//
		PopLuaData(ls, ParamOut.GetParam(i), nPos);
	}

	int nCount = lua_gettop(ls);

	lua_settop(ls, -1-ParamOut.GetCount());		//出栈

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

//参数从左到右参数进栈
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

		//tolua_pushusertype(pState,pVoid,pParamIn->GetType());		//压入栈中，

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

//预先加载luabuf 
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
