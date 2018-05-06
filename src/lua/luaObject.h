#ifndef _LUA_OBJECT_H_
#define _LUA_OBJECT_H_

#include"common\common.h"
#include"lua\luaTools.h"

class LuaContext;

/**
*	\brief luaObject �ṩ��C++������lua userdata�ķ�ʽ����
*/
class LuaObject : public std::enable_shared_from_this<LuaObject>
{
public:
	LuaObject();
	virtual ~LuaObject();

	LuaContext* GetLuaContext();
	void SetLuaContext(LuaContext* luaContext);

	bool IsKnowToLua()const;
	void SetKnowToLua(bool knowToLua);

	bool IsWithLuaTable()const;
	void SetWithLuaTable(bool withLuaTable);

	virtual const string GetLuaObjectName()const = 0;
	
	/** lua bind function */
	int LuaWraper(lua_State*l);

private:
	LuaContext* mLuaContext;

	bool mWithLuaTable; // �Ƿ���Ϊһ��lua table
	bool mKnowToLua;	// �Ƿ��Ѿ����뵽lua

};

using LuaObjectPtr = std::shared_ptr<LuaObject>;


#endif