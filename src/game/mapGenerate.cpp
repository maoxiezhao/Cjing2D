#include"mapGenerate.h"
#include"game\map.h"
#include"core\random.h"
#include"core\fileData.h"
#include"core\video.h"
#include"gui\core\uiRender.h"

bool MapGenerateData::ExportToLua(lua_State * l)
{
	return false;
}

bool MapGenerateData::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "mapGenrateData");
	lua_register(l, "mapInfo", LuaMapGenerateData);
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Faild to load map generate data.");
		lua_pop(l, 1);
		return false;
	}
	return true;
}

/**
*	\brief 添加房间信息
*	\param 房间的ID
*	\param 房间的描述信息
*/
void MapGenerateData::AddRoomInfo(const std::string& roomID, MapRoomInfo roomInfo)
{
	mRoomInfos[roomID] = roomInfo;
}

/**
*	\brief 解析地图生成数据
*/
int MapGenerateData::LuaMapGenerateData(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "mapGenrateData");
		MapGenerateData* mapGenerateData = static_cast<MapGenerateData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		const std::string name = LuaTools::CheckFieldString(l, 1, "name");
		int x = LuaTools::CheckFieldInt(l, 1, "x");
		int y = LuaTools::CheckFieldInt(l, 1, "y");
		int width = LuaTools::CheckFieldInt(l, 1, "width");
		int height = LuaTools::CheckFieldInt(l, 1, "height");
		int minLayer = LuaTools::CheckFieldInt(l, 1, "minlayer");
		int maxLayer = LuaTools::CheckFieldInt(l, 1, "maxlayer");
		int minRooms = LuaTools::CheckFieldInt(l, 1, "minRooms");
		int maxRooms = LuaTools::CheckFieldInt(l, 1, "maxRooms");
		bool isRandom = LuaTools::CheckFieldBool(l, 1, "random");

		mapGenerateData->SetMapID(name);
		mapGenerateData->SetPos({ x, y });
		mapGenerateData->SetSize({ width, height });
		mapGenerateData->SetMinLayer(minLayer);
		mapGenerateData->SetMaxLayer(maxLayer);
		mapGenerateData->SetMinRooms(minRooms);
		mapGenerateData->SetMaxRooms(maxRooms);
		mapGenerateData->SetRandomGenerate(isRandom);

		// 加载房间信息
		lua_settop(l, 1);
		lua_getfield(l, 1, "rooms");
		if (lua_type(l, 2) != LUA_TTABLE)
		{
			LuaTools::ArgError(l, 1,
				string("Bad filed 'rooms',(excepted table,got ") + lua_typename(l, -1) + ")");
		}
		int roomIndex = 1;
		lua_rawgeti(l, 2, roomIndex);
		while (!lua_isnil(l, -1))
		{
			MapGenerateData::MapRoomInfo roomInfo;
			roomInfo.name = LuaTools::CheckFieldString(l, -1, "name");
			const std::string curPath = mapGenerateData->GetFilePath();
			const std::string roomPath = LuaTools::CheckFieldString(l, -1, "path");
			roomInfo.path = FileData::GetPositivePath(curPath, roomPath);
			
			int x = LuaTools::CheckFieldIntByDefault(l, -1, "x", 0);
			int y = LuaTools::CheckFieldIntByDefault(l, -1, "y", 0);
			roomInfo.pos = Point2(x, y);
			roomInfo.count = LuaTools::CheckFieldInt(l, -1, "count");
			roomInfo.linkMask = LuaTools::CheckFieldStringByDefault(l, -1, "linkMask", "0000");
			mapGenerateData->AddRoomInfo(roomInfo.name, roomInfo);

			roomIndex++;
			lua_pop(l, 1);
			lua_rawgeti(l, -1, roomIndex);
		}
		lua_settop(l, 1);
		return 0;
	});
}


MapGenerate::MapGenerate():
	mMapID(""),
	mMinLayer(0),
	mMaxLayer(0),
	mRoomCount(0)
{
}

MapGenerate::~MapGenerate()
{
	Clear();
}

/**
*	\brief 加载地图信息,同时根据标记是否随机生成地图
*/
bool MapGenerate::LoadMap(const std::string & mapID)
{
	// 首先如果存在存档，则加载已经生成的地图

	// 加载总地图信息
	MapGenerateData generateData;
	const string mapGenerateName = string("maps/") + mapID + "/mapInfo.dat";
	bool successed = generateData.ImportFromFile(mapGenerateName);
	if (!successed)
		return false;

	mMapID = generateData.GetMapID();
	mPos = generateData.GetPos();
	mSize = generateData.GetSize();
	mMinLayer = generateData.GetMinLayer();
	mMaxLayer = generateData.GetMaxLayer();
	mRoomCount = 0;
	
	// 加载各个房间信息
	const auto& roomInfos = generateData.GetRoomInfos();
	for (auto& kvp : roomInfos)
	{
		// mapData
		const auto& roomInfo = kvp.second;
		int count = roomInfo.count;
		if(count < 0)
			continue;

		// 添加到roomInfos
		MapDataPtr mapData = std::make_shared<MapData>();
		if (!mapData->ImportFromFile(roomInfo.path + ".dat"))
		{
			Debug::Error("Failed to load room'" + roomInfo.path + "' data");
			continue;
		}
		AddMapData(roomInfo.name, mapData, count);
	}

	// 随机生成地图
	bool randomGenerate = generateData.GetRandomGenerate();
	bool result = true;
	if (randomGenerate)
		result = RandomGenerateMap();

	return result;
}

/**
*	\brief 保存当前生成的随机地图
*/
bool MapGenerate::SaveMap()
{
	return false;
}

/**
*	\brief debug绘制
*/
void MapGenerate::DrawDebug()
{
	const Size screenSize = Video::GetScreenSize();
	float ratioX = (float)mSize.width / screenSize.width;
	float ratioY = (float)mSize.height / screenSize.height;
	GLubyte stepColor = 255 / mRoomCount;
	GLubyte index = 0;
	// draw debug room
	for (auto mapID : mMapRoomIDs)
	{
		const auto& mapRects = mMapRoomRect[mapID];
		for (auto rect : mapRects)
		{
			gui::UIRender::RenderShape(
			{ (int)(rect.x / ratioX), 
			  (int)(rect.y / ratioY), 
			  (int)(rect.width / ratioX),
			  (int)(rect.height / ratioY) },
			{ (GLubyte)(stepColor * (mRoomCount - index)),255, (GLubyte)(stepColor * index),255 });
			index++;
		}
	}
}

void MapGenerate::Clear()
{
	mMapRoomIDs.clear();
	mMapRooms.clear();
	mMapRoomCount.clear();
	mMapRoomRect.clear();
}

std::unordered_map<string, int> MapGenerate::GetMapRoomCount() const
{
	return mMapRoomCount;
}

std::vector<string> MapGenerate::GetMapRoomIDs() const
{
	return std::vector<string>();
}

const std::unordered_map<string, MapDataPtr>& MapGenerate::GetMapRooms()const
{
	return mMapRooms;
}

namespace {
	/**
	*	\brief 随机排布房间
	*/
	bool LayoutRoomRects(std::vector<Rect>& roomRects, Rect boundRect, int tryTimes)
	{
		auto RectValidCheck = [](const std::vector<Rect>& rects, const Rect& testRect)->bool {
			for (const auto& rect : rects)
				if (testRect.Overlaps(rect))
					return false;
			return true;
		};

		// 随机生成各个room的rect的位置
		int curTimes = 0;
		int cellSize = MapGenerate::cellSize;
		std::vector<Rect> rects;
		for (auto& rect : roomRects)
		{
			while (1){
				Rect tmpRect = rect;
				int randPosX = Random::GetRandomInt(boundRect.width - tmpRect.width - cellSize) + boundRect.x + 1;
				int randPosY = Random::GetRandomInt(boundRect.height - tmpRect.height - cellSize) + boundRect.y + 1;
				randPosX = (randPosX & (cellSize - 1)) > 0 ? randPosX - (randPosX & (cellSize - 1)) + cellSize : randPosX;
				randPosY = (randPosY & (cellSize - 1)) > 0 ? randPosY - (randPosY & (cellSize - 1)) + cellSize : randPosY;
				tmpRect.SetPos(randPosX, randPosY);

				if (RectValidCheck(rects, tmpRect))
				{
					rects.push_back(tmpRect);
					break;
				}

				// 当尝试次数过大时直接错误返回
				curTimes++;
				if (curTimes >= tryTimes)
				{
					Debug::Error("Failed to layout room, the max try times:" + std::to_string(tryTimes));
					return false;
				}
			}
		}

		roomRects.clear();
		for (auto rect : rects)
			roomRects.push_back(rect);

		return true;
	}
}

void MapGenerate::AddMapData(const std::string & name, MapDataPtr mapData, int count)
{
	mMapRoomIDs.push_back(name);
	mMapRooms[name] = mapData;
	mMapRoomCount[name] = count;
	mRoomCount += count;
}

/**
*	\brief 随机生成地图过程
*
*	对于当前版本的（第一版）的地图随机生成算法，主要处理两点
*	1.是根据随机生成的房间位置，动态设置对应room的mapData的位置
*	2.根据生成后的位置，动态创建走廊
*
*	暂时不考虑房间之间的相斥距离，例如出生点和boss房的生成距离
*/
bool MapGenerate::RandomGenerateMap()
{
	util::Grid<int> grids(mSize, { cellSize, cellSize });
	grids.SetCellCapacity(1);

	// 0.添加room rect
	std::vector<Rect> roomRect;
	for (auto mapID : mMapRoomIDs)
	{
		MapDataPtr mapData = mMapRooms[mapID];		// 不需要判空，必然存在
		int count = mMapRoomCount[mapID];		
		for (int i = 0; i < count; i++)
		{
			Rect rect = mapData->GetRect();
			roomRect.push_back(rect);
		}
	}

	// 1.不断尝试随机排布位置
	if (!LayoutRoomRects(roomRect, Rect(mPos, mSize), 50))
	{
		Debug::Error("Failed to layout room.");
		return false;
	}

	int index = 0;
	for (auto mapID : mMapRoomIDs)
	{
		auto& mapRects = mMapRoomRect[mapID];		
		int count = mMapRoomCount[mapID];
		for (int i = 0; i < count; i++)
		{
			if (index >= roomRect.size())
			{
				Debug::Error("Error in Layout Room rects.");
				return false;
			}
			// 修改mapData位置数据,同时添加到grid中
			Rect rect = roomRect[index];
			mapRects.push_back(rect);
			grids.Add(index, rect);
			index++;
		}
	}

	// 2.添加走廊数据
	if (!RandomGenerateHallWay())
	{
		Debug::Error("Failed to generate hallway.");
		return false;
	}

	// 3.额外的修饰

	return true;
}

/**+
*	\brief 随机生成走廊
*
*	洪水填充法创建走廊
*/
bool MapGenerate::RandomGenerateHallWay()
{
	// 1.遍历全图通过栈实现洪水填充走廊

	// 2.链接走廊和房间

	// 3.删除死胡同

	return true;
}


