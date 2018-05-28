#include "mapData.h"
#include "core\debug.h"
#include "lua\luaContext.h"
#include <sstream>

MapData::MapData():
	mMapName(""),
	mMinLayer(0),
	mMaxLayer(0),
	mSize(),
	mPosition(),
	mTilesetID("")
{
}

bool MapData::ExportToLua(lua_State * l)
{
	return false;
}

void MapData::SetPosition(const Point2 & pos)
{
	mPosition = pos;
}

const Point2 & MapData::GetPosition() const
{
	return mPosition;
}

void MapData::SetMinLayer(int minLayer)
{
	mMinLayer = minLayer;
}

int MapData::GetMinLayer() const
{
	return mMinLayer;
}

void MapData::SetMaxLayer(int maxLayer)
{
	mMaxLayer = maxLayer;
}

int MapData::GetMaxLayer() const
{
	return mMaxLayer;
}

void MapData::SetSize(const Size & size)
{
	mSize = size;
}

Size MapData::GetSize() const
{
	return mSize;
}

void MapData::SetTitlesetID(const string & id)
{
	mTilesetID = id;
}

const string & MapData::getTitlesetID() const
{
	return mTilesetID;
}

bool MapData::IsValidLayer(int layer) const
{
	return (layer >= mMinLayer && 
		layer <= mMaxLayer);
}

Rect MapData::GetRect() const
{
	return Rect(mPosition, mSize);
}

string MapData::GetMapPath() const
{
	return mMapPath;
}

void MapData::SetMapPath(const std::string & path)
{
	mMapPath = path;
}

/**
*	\brief ���һ��entityData
*	\return ��ӳɹ�����true,��֮����false
*/
int MapData::GetEntityCountByLayer(int layer)const
{
	Debug::CheckAssertion(layer >= mMinLayer && layer <= mMaxLayer,
		"The getting entity layer is Invalid layer.");

	if (layer >= static_cast<int>(mEntitiesByLayer.size()) )
		return 0;
	else
		return mEntitiesByLayer.at(layer).size();
}

bool MapData::AddEntity(const EntityData & entityData)
{
	int layer = entityData.GetLayer();
	Debug::CheckAssertion(layer >= mMinLayer && layer <= mMaxLayer,
		"The adding entity has a invalid layer.");

	mEntitiesByLayer[layer].push_back(entityData);
	return true;
}

const EntityData & MapData::GetEntity(int layer, int index) const
{
	Debug::CheckAssertion(layer >= mMinLayer && layer <= mMaxLayer,
		"The getting entity layer is Invalid layer");
	
	const auto& entityList = mEntitiesByLayer.at(layer);
	Debug::CheckAssertion(index >= 0 && index < static_cast<int>(entityList.size()),
		"The  getting entity has a invalid index.");

	return entityList.at(index);
}

/**
*	\brief ��.dat��ͼ�ļ��м�������
*/
bool MapData::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "mapData");
	lua_register(l, "property", LuaMapDataProperty);
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Faild to load map data.");
		lua_pop(l, 1);
		return false;
	}
	return true;
}

/**
*	\brief ������ͼproperty
*/
int MapData::LuaMapDataProperty(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "mapData");
		MapData* mapData = static_cast<MapData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		int x = LuaTools::CheckFieldInt(l, 1, "x");
		int y = LuaTools::CheckFieldInt(l, 1, "y");
		int width = LuaTools::CheckFieldInt(l, 1, "width");
		int height = LuaTools::CheckFieldInt(l, 1, "height");
		int minLayer = LuaTools::CheckFieldInt(l, 1, "min_layer");
		int maxLayer = LuaTools::CheckFieldInt(l, 1, "max_layer");
		const string& tilesetID = LuaTools::CheckFieldString(l, 1, "tileset");

		mapData->SetPosition({ x, y });
		mapData->SetSize({width, height});
		mapData->SetMinLayer(minLayer);
		mapData->SetMaxLayer(maxLayer);
		mapData->SetTitlesetID(tilesetID);

		// ��Ӹ���entity�Ľ�������
		for ( const auto& entityDescription : EntityData::GetEntityFieldDescriptions())
		{
			const std::string& typeName = EnumToString(entityDescription.first);
			lua_pushstring(l, typeName.c_str());
			lua_pushcclosure(l, LuaAddEntity, 1);
			lua_setglobal(l, typeName.c_str());
		}

		return 0;
	});
}

/**
*	\brief ������entity����ת����
*
*	�ú�����LuaMapDataProperty��ȷ��������perperty���ã�ͨ�����������
*   ��entitytype��Ϊ�հ�ѹջ��������ͨ����ֵ��ȡ���ö�Ӧ�Ĵ�������
*/
int MapData::LuaAddEntity(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "mapData");
		MapData* mapData = static_cast<MapData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		const string& typeStr = LuaTools::CheckString(l, lua_upvalueindex(1));
		EntityType entityType = StringToEnum<EntityType>(typeStr);	
		EntityData entityData = EntityData::CheckEntityData(l, 1, entityType);

		// ���entity�Ƿ��ںϷ��Ĳ㼶
		if (!mapData->IsValidLayer(entityData.GetLayer()) )
		{
			std::ostringstream oss;
			oss << "Invalid layer: " << entityData.GetLayer();
			LuaTools::Error(l, oss.str());
		}

		if (!mapData->AddEntity(entityData) )
		{
			std::ostringstream oss;
			oss << "Failed to add entity " << entityData.GetName();
			LuaTools::Error(l, oss.str());
		}
		return 0;
	});
}
