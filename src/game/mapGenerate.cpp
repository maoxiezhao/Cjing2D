#include"game\mapGenerate.h"
#include"game\map.h"
#include"core\random.h"
#include"core\fileData.h"
#include"core\video.h"
#include"core\random.h"
#include"gui\core\uiRender.h"

#include<stack>
#include<algorithm>

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
	mRoomCount(0),
	mRandomGenerateHallWay(true)
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
	return;

	const Size screenSize = Video::GetScreenSize();
	float ratioX = (float)mSize.width / screenSize.width;
	float ratioY = (float)mSize.height / screenSize.height;
	GLubyte stepColor = 255 / mRoomCount;
	GLubyte index = 0;
	// draw debug grid
	int gridCols = mSize.width / cellSize + 1;
	int gridRows = mSize.height / cellSize + 1;
	for (int y = 0; y < gridRows; y++)
	{
		for (int x = 0; x < gridCols; x++)
		{
			gui::UIRender::RenderShape(
			{   (int)(x * cellSize / ratioX),
				(int)(y * cellSize / ratioY),
				(int)((cellSize - 16) / ratioX),
				(int)((cellSize - 16) / ratioY) },
				{ 255,255, 255,55 });

			int region = mDebugGrid[y * gridCols + x];
			if(region >= 0)
				gui::UIRender::RenderText(std::to_string(region), 
				(int)(x * cellSize / ratioX), 
				(int)(y * cellSize / ratioY));
		}
	}

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
			{ (GLubyte)(stepColor * (mRoomCount - index)),255, (GLubyte)(stepColor * index),100 });
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
			Rect tmpRect(testRect);
			tmpRect.AddSize(MapGenerate::cellSize, MapGenerate::cellSize);
			for (auto rect : rects) {
				rect.AddSize(MapGenerate::cellSize, MapGenerate::cellSize);
				if (tmpRect.Overlaps(rect))
					return false;
			}
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
				randPosX = ((randPosX / cellSize + 1) | 1 )* cellSize;
				randPosY = ((randPosY / cellSize + 1) | 1 )* cellSize;
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
	if (!LayoutRoomRects(roomRect, Rect(mPos, mSize), 500))
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
	if (mRandomGenerateHallWay && !RandomGenerateHallWay(grids))
	{
		Debug::Error("Failed to generate hallway by random way.");
		return false;
	}
	else if (!mRandomGenerateHallWay && QuickGenerateHallWay(grids))
	{
		Debug::Error("Failed to generate hallway by directly way.");
		return false;
	}

	// 3.额外的修饰

	return true;
}

namespace {
	int regionCount = 0;
	enum FloorFillDir {
		FloorFillNone,
		FloorFillUp,
		FloorFillRight,
		FloorFillDown,
		FloorFillLeft,
	};

	Point2 GetPosByDir(const Point2& pos, FloorFillDir dir, int step = 1){
		Point2 point = pos;
		switch (dir) {
		case FloorFillUp:   point.y-= step; break;
		case FloorFillRight:point.x+= step; break;
		case FloorFillDown: point.y+= step; break;
		case FloorFillLeft: point.x-= step; break;
		}
		return point;
	};

	/**
	*	\brief 洪水填充发，房间的大小和位置已经道路的
	*	生成节点都必须是奇数。
	*/
	void FloorFill(const Point2& pos, util::Grid<int>& grid, 
		std::function<bool(const Point2& pos, FloorFillDir dir, util::Grid<int>& grid)> checkFunc)
	{
		if (!checkFunc(pos, FloorFillDir::FloorFillNone, grid))
			return;

		bool isAddCell = false;
		std::stack<Point2> mFindCells;
		mFindCells.push(pos);
		FloorFillDir lastDir = FloorFillNone;
		while (!mFindCells.empty())
		{
			Point2 lastPos = mFindCells.top();
			mFindCells.pop();

			if (!checkFunc(lastPos, FloorFillDir::FloorFillNone, grid))
				continue;;

			//grid.Add(regionCount, lastPos.x, lastPos.y);

			std::vector<FloorFillDir> dirs;
			for (int i = FloorFillUp; i <= FloorFillLeft; i++)
			{
				// 这里需要一个特殊处理，避免产生一个联通的单独格子
				auto firstPos = GetPosByDir(lastPos, static_cast<FloorFillDir>(i));
				//auto secondPos = GetPosByDir(firstPos, static_cast<FloorFillDir>(i));
				if (checkFunc(firstPos, static_cast<FloorFillDir>(i), grid)) // && checkFunc(secondPos, grid))
					dirs.push_back(static_cast<FloorFillDir>(i));
			}

			if (!dirs.empty())
			{
				FloorFillDir dir = FloorFillNone;
				bool bContainer = std::find(dirs.begin(), dirs.end(), lastDir) != dirs.end();
				if (bContainer && Random::GetRandomFloat() >= 0.0f)
				{
					dir = lastDir;
					std::random_shuffle(dirs.begin(), dirs.end());
					for (auto curDir : dirs)
					{
						if(curDir != lastDir)
							mFindCells.push(GetPosByDir(lastPos, curDir));
					}
					mFindCells.push(GetPosByDir(lastPos, dir));
				}
				else
				{	
					std::random_shuffle(dirs.begin(), dirs.end());
					for (auto curDir : dirs)
					{
						mFindCells.push(GetPosByDir(lastPos, curDir));
					}
					dir = dirs.at(dirs.size() - 1);
				}
				isAddCell = true;
				grid.Add(regionCount, lastPos.x, lastPos.y);
				lastDir = dir;
			}
		}

		// 区域加一
		if(isAddCell)
			regionCount++;
	}
}

/**
*	\brief 随机生成走廊
*/
bool MapGenerate::RandomGenerateHallWay(util::Grid<int>& grid)
{
	// 创建区域，首先添加room区域
	std::vector<int> regions;
	for (int i = 0; i < mRoomCount; i++)
		regions.push_back(i);

	// 1.遍历全图通过栈实现洪水填充走廊
	// 走廊填充检查函数，如果周围
	int roomRegionCount = mRoomCount;
	auto CheckFunc = [roomRegionCount](const Point2& pos, FloorFillDir dir, util::Grid<int>& grid)->bool {
		if (!grid.GetElements(pos.x, pos.y).empty())
			return false;
		if (pos.x < 0 || pos.x >= grid.GetCols() || pos.y < 0 || pos.y >= grid.GetRows())
			return false;

		if (dir == FloorFillDir::FloorFillNone) {
			// 仅相连一面为空或者未相连
			int wallCount = 0;

			auto vecs = grid.GetElements(pos.x + 1, pos.y);
			if (!vecs.empty())if (vecs[0] >= roomRegionCount)wallCount++; else return false;;
			vecs = grid.GetElements(pos.x - 1, pos.y);
			if (!vecs.empty())if (vecs[0] >= roomRegionCount)wallCount++; else return false;
			vecs = grid.GetElements(pos.x, pos.y + 1);
			if (!vecs.empty())if (vecs[0] >= roomRegionCount)wallCount++; else return false;
			vecs = grid.GetElements(pos.x, pos.y - 1);
			if (!vecs.empty())if (vecs[0] >= roomRegionCount)wallCount++; else return false;

			return wallCount <= 1;
		}
		else
		{
			// 如果存在方向，则必然肯定存在一个连接点，只要判断3个点即
			auto firstPos = GetPosByDir(pos, dir);
			if (!grid.GetElements(firstPos.x, firstPos.y).empty())
				return false;
			dir = (dir == FloorFillUp) ? FloorFillDir::FloorFillLeft : static_cast<FloorFillDir>(dir - 1);
			auto secondPos = GetPosByDir(firstPos, dir);
			if (!grid.GetElements(secondPos.x, secondPos.y).empty())
				return false;
			dir = (dir == FloorFillLeft) ? FloorFillDir::FloorFillUp : static_cast<FloorFillDir>(dir + 1);
			secondPos = GetPosByDir(firstPos, dir);
			if (!grid.GetElements(secondPos.x, secondPos.y).empty())
				return false;
		}
		return true;

	};
	regionCount = mRoomCount;
	int cols = grid.GetCols();
	int rows = grid.GetRows();
	for (int y = 0; y < rows; y++){
		for (int x = 0; x < cols; x++){
			FloorFill({ x, y }, grid, CheckFunc);
		}
	}
	// 添加hallway 区域
	for (int i = mRoomCount; i < regionCount; i++)
		regions.push_back(i);

	// 1.生成走廊，直接直线生成

	// 2.链接走廊和房间
	if (!ConnectRegions(grid, regions))
	{
		Debug::Error("Failed to connect regions mapID:" + mMapID);
		return false;
	}

	// 3.删除死胡同
	if (!RemoveDeadWay(grid))
	{
		Debug::Error("Failed to remove dead way.mapID:" + mMapID);
		return false;
	}

	// debug test
	mDebugGrid.clear();
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			auto v = grid.GetElements(x, y);
			if (v.empty())
				mDebugGrid.push_back(-1);
			else
				mDebugGrid.push_back(v[0]);
		}
	}
	

	return true;
}

/**
*	\brief 以直线或者折线方式快速链接房间
*/
bool MapGenerate::QuickGenerateHallWay(util::Grid<int>& grid)
{
	return false;
}

/**
*	\brief 链接各个区域
*/
bool MapGenerate::ConnectRegions(util::Grid<int>& grid, const std::vector<int>& regions)
{
	return true;

	// 1. 首先添加可作为通道的连接点
	auto CheckConnection = [&grid](int x, int y)->bool {
		auto v = grid.GetElements(x, y);
		if (v.empty())
			return false;

	};
	std::vector<Point2> mConnectors;
	int cols = grid.GetCols();
	int rows = grid.GetRows();
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (CheckConnection(x, y))
				mConnectors.emplace_back(x, y);
		}
	}

	return false;
}

/**
*	\brief 移除死胡同
*/
bool MapGenerate::RemoveDeadWay(util::Grid<int>& grid)
{
	return true;
}


