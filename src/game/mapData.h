#pragma once

#include"common\common.h"
#include"lua\luaData.h"
#include"utils\size.h"
#include"utils\point.h"
#include"utils\rectangle.h"
#include"entity\EntityData.h"

#include<deque>

/**
*	\brief 地图数据描述
*/

using EntityDataList = std::deque<EntityData>;

class MapData : public LuaData
{
public:
	MapData();

	virtual bool ImportFromLua(lua_State*l);
	virtual bool ExportToLua(lua_State*l);

	/**** **** **** setter/getter **** **** ****/
	void SetPosition(const Point2& pos);
	const Point2& GetPosition()const;
	void SetMinLayer(int minLayer);
	int GetMinLayer()const;
	void SetMaxLayer(int maxLayer);
	int GetMaxLayer()const;
	void SetSize(const Size& size);
	Size GetSize()const;
	void SetTitlesetID(const string& id);
	const string& getTitlesetID()const;
	bool IsValidLayer(int layer)const;
	Rect GetRect()const;
	string GetMapPath()const;
	void SetMapPath(const std::string& path);

	/***** ***** ****** entity ***** ***** ******/
	int GetEntityCountByLayer(int layer)const;
	bool AddEntity(const EntityData& entityData);
	const EntityData& GetEntity(int layer, int index)const;

private:
	static int LuaMapDataProperty(lua_State* l);
	static int LuaAddEntity(lua_State* l);

private:
	string mMapName;
	int mMinLayer;								
	int mMaxLayer;	
	Size mSize;
	Point2 mPosition;
	string mTilesetID;
	string mMapPath;
	std::map<int, EntityDataList> mEntitiesByLayer;	/** 存储每层的entity数据信息 */

};

using MapDataPtr = std::shared_ptr<MapData>;