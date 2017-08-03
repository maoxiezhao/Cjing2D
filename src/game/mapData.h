#pragma once

#include"common\common.h"
#include"lua\luaData.h"
#include"utils\size.h"
#include"utils\point.h"
#include"game\EntityData.h"

#include<deque>

using EntityDataList = std::deque<EntityData>;

class MapData : public LuaData
{
public:
	MapData();

	virtual bool ImportFromLua(lua_State*l);
	virtual bool ExportToLua(lua_State*l);

private:
	string mMapName;
	int mMinLayer;								
	int mMaxLayer;	
	Size mSize;
	Point2 mPosition;
	string mTilesetID;

	std::map<int, EntityDataList> mEntitiesByLayer;	/** 存储每层的entity数据信息 */

};