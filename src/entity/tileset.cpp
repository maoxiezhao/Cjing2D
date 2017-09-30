#include "tileset.h"
#include "entity\noAnimatedTilePattern.h"
#include "entity\animatedTilePattern.h"
#include "entity\groundInfo.h"

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

/**
*	\brief 解析tilesetPattern数据
*
*	接续tilesetPattern时，需要考虑noanimated pattern和animated pattern
*/
int TilesetData::LuaTilesetData(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
		TilesetData& tilesetData = *static_cast<TilesetData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		int id = LuaTools::CheckFieldInt(l, 1, "id");
		int width = LuaTools::CheckFieldInt(l, 1, "width");
		int height = LuaTools::CheckFieldInt(l, 1, "height");
		const std::string groundStr = LuaTools::CheckFieldStringByDefault(l, 1, "ground", "");
		Ground ground = StringToEnum<Ground>(groundStr, Ground::GROUND_EMPTY);

		// 解析pattern帧数据
		int xFrameCount = 0;
		int yFrameCount = 0;
		std::vector<Rect> frameRects;		
		// x data
		lua_settop(l, 1);
		lua_getfield(l, 1, "x");
		if (lua_isnumber(l, 2))
		{
			int x = LuaTools::CheckInt(l, 2);
			Rect frameRect(x, x, width, height);
			frameRects.push_back(frameRect);
			lua_pop(l, 1);
		}
		else if (lua_istable(l, 2))
		{
			lua_pushnil(l);
			while(lua_next(l, 2) != 0)
			{				// pattern table key value
				int x = LuaTools::CheckInt(l, -1);
				
				Rect frameRect(x, x, width, height);
				frameRects.push_back(frameRect);
				xFrameCount++;
				lua_pop(l, 1);
			}
			lua_pop(l, 1);
		}
		else
		{
			LuaTools::Error(l, "Invalid tile pattern x data.");
		}
		// y data
		lua_getfield(l, 1, "y");
		if (lua_isnumber(l, 2))
		{
			int y = LuaTools::CheckInt(l, 2);
			frameRects[yFrameCount].SetPosY(y);
			lua_pop(l, 1);
		}
		else if (lua_istable(l, 2))
		{
			lua_pushnil(l);
			while (lua_next(l, 2) != 0)
			{				// pattern table key value
				int y = LuaTools::CheckInt(l, -1);
				if (yFrameCount >= static_cast<int>(frameRects.size()))
					LuaTools::Error(l, "x frame count must equal to y frame count.");

				frameRects[yFrameCount].SetPosY(y);
				yFrameCount++;
				lua_pop(l, 1);
			}
			lua_pop(l, 1);
		}
		else
		{
			LuaTools::Error(l, "Invalid tile pattern y data.");
		}

		Debug::CheckAssertion(lua_gettop(l) == 1,
			"Something in stack after parsed tile pattern data.");
		Debug::CheckAssertion(xFrameCount == yFrameCount,
				"x frame count must equal to y frame count.");

		// 创建tilePatterns
		TilePatternData tilePattern;
		tilePattern.SetPatternID(id);
		tilePattern.SetGround(ground);
		tilePattern.SetSize({ width, height });
		tilePattern.SetFrameRect(frameRects);
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
	if (data.GetFrameRect().size() == 1)
	{	// 单帧Tilepattern
		const auto& frameRects = data.GetFrameRect();
		TilePattern* pattern = new NoAnimatedTilePattern(data);
		pattern->SetPos(frameRects[0].GetPos());
		mTilePatterns.emplace(id, std::unique_ptr<TilePattern>(pattern));
	}
	else
	{	// 多帧Tilepattern
		const auto& frameRects = data.GetFrameRect();
		AnimatedTilePattern* pattern = new AnimatedTilePattern(data);
		pattern->SetFrameRect(frameRects);
		mTilePatterns.emplace(id, std::unique_ptr<TilePattern>(pattern));
	}
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