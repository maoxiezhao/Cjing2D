#ifndef _LUA_OBJECT_H_
#define _LUA_OBJECT_H_

#include"common\common.h"
#include"lua\luaTools.h"

class LuaContext;

/**
*	\brief luaObject 提供了C++对象以lua userdata的方式存在
*/
class LuaObject : public std::enable_shared_from_this<LuaObject>
{
public:
	LuaObject();
	virtual ~LuaObject();

	LuaContext* GetLuaContext()const;
	void SetLuaContext(LuaContext* luaContext);

	bool IsKnowToLua()const;
	void SetKnowToLua(bool knowToLua);

	bool IsWithLuaTable()const;
	void SetWithLuaTable(bool withLuaTable);

	virtual const string GetLuaObjectName()const = 0;

private:
	LuaContext* mLuaContext;

	bool mWithLuaTable; // 是否作为一个lua table
	bool mKnowToLua;	// 是否已经载入到lua

};

using LuaObjectPtr = std::shared_ptr<LuaObject>;


#endif