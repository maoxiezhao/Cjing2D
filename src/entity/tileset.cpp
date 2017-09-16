#include "tileset.h"

bool TilesetData::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "tileset");
	lua_register(l, "tilePattern", LuaTilesetData);
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Failed to load tileset datafile.");
		lua_pop(l, 1);	// ?????
		return false;
	}
	return true;
}

const std::vector<TilePatternData>& TilesetData::GetTilePatterns()
{
	return mTilePatterns;
}

void TilesetData::PushTilePattern(const TilePatternData & data)
{
	mTilePatterns.push_back(data);
}

int TilesetData::LuaTilesetData(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
		TilesetData& tilesetData = *static_cast<TilesetData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		int id = LuaTools::CheckFieldInt(l, 1, "id");
		int x = LuaTools::CheckFieldInt(l, 1, "x");
		int y = LuaTools::CheckFieldInt(l, 1, "y");
		int width = LuaTools::CheckFieldInt(l, 1, "width");
		int height = LuaTools::CheckFieldInt(l, 1, "height");

		TilePatternData tilePattern;
		tilePattern.SetPatternID(id);
		tilePattern.SetPos({ x, y });
		tilePattern.SetSize({ width, height });

		tilesetData.PushTilePattern(tilePattern);
		return 0;
	});
}

Tileset::Tileset(const string & id):
	mTilesetID(id),
	mTilesImage(nullptr)
{
}

/**
*	\brief 加载tileset data信息
*
*	tileset data信息包含了tiles image和tile patterns
*/
void Tileset::Load()
{
	// tileset data
	std::string dataPath = string("tilesets/") + mTilesetID + ".dat";
	TilesetData tilesetData;
	tilesetData.ImportFromFile(dataPath);
	for (const auto& pattern : tilesetData.GetTilePatterns())
	{
		AddTilePattern(pattern.GetPatternID(), pattern);
	}

	// tileset image
	std::string imgPath = string("tilesets/") + mTilesetID + ".png";
	mTilesImage = std::make_shared<Sprite>(imgPath);
	if (mTilesImage == nullptr)
	{
		Debug::Error("Failed to load tileset image '" + mTilesetID + "'");
	}
}

void Tileset::UnLoad()
{
	mTilePatterns.clear();
}

/**
*	\brief 添加tilePattern,目前仅支持简单pattern
*/
void Tileset::AddTilePattern(int id, const TilePatternData & data)
{
	TilePattern* pattern = new TilePattern(data);
	mTilePatterns.emplace(id, std::unique_ptr<TilePattern>(pattern));
}

/**
*	\brief 根据id获取对应的pattern
*	\param id 需要获取pattern的id
*
*	当传入非法的id时，会直接抛出异常
*/
const TilePattern & Tileset::GetTilePattern(int id) const
{
	const auto& it = mTilePatterns.find(id);
	if (it == mTilePatterns.end())
	{
		std::ostringstream oss;
		oss << "Invalid tile id '" << id << "'";
		Debug::Error(oss.str());
	}
	return *it->second;
}

SpritePtr Tileset::GetTileImage()const
{
	return mTilesImage;
}