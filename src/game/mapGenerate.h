#pragma once

#include"game\mapData.h"
#include"utils\grid.h"
#include<unordered_map>

/**
*	\brief 随机地图生成
*/
/**
*	\brief MapGenerateData
*
*	MapGenerateData决定了当前地图的相关信息，同时包含了当前地图的相关
*	房间信息（mapData)
*/
class MapGenerateData : public LuaData
{
public:
	MapGenerateData() = default;

	virtual bool ImportFromLua(lua_State*l);
	virtual bool ExportToLua(lua_State*l);
	static int LuaMapGenerateData(lua_State*l);

	/**
	*	\brief 地图房间信息
	*/
	struct MapRoomInfo {
		std::string name;
		std::string path;
		Point2 pos;
		int count;
		std::string linkMask;
	};

	/** setter/getter */
	SetterGetterMethod(std::string, MapID)
	SetterGetterMethod(Point2, Pos)
	SetterGetterMethod(Size, Size)
	SetterGetterMethod(int, MinLayer)
	SetterGetterMethod(int, MaxLayer)
	SetterGetterMethod(int, MinRooms)
	SetterGetterMethod(int, MaxRooms)
	SetterGetterMethod(bool, RandomGenerate)
	SetterGetterMethod(bool, RandomHallWay)
	SetterGetterMethod(std::string, TilesetID)

	void AddRoomInfo(const std::string& roomID, MapRoomInfo roomInfo);
	const std::map<string, MapRoomInfo>& GetRoomInfos()const { return mRoomInfos; }
	
private:
	// base property
	std::string mMapID;
	Point2 mPos;
	Size mSize;
	int mMinLayer, mMaxLayer;
	int mMinRooms, mMaxRooms;
	bool mRandomGenerate;
	std::string mTilesetID;

	// hallway data
	bool mRandomHallWay;

	// room data
	std::map<string, MapRoomInfo> mRoomInfos;

	// hallway path
	std::vector<std::string> mHallwayPaths;
};

/**
*	\brief 地图随机生成功能试做
*
*	从地图模板库中随机取出对应的地图房间，然后随机排布在地图中，
*	使用洪水填充法，绘制通道，再链接各个通道、房间，存储并提供给
*   Map class管理
*
*	TODO 生成逻辑有点混乱，需要重构 18/5/19
*/
class MapGenerate
{
public:
	MapGenerate();
	~MapGenerate();

	bool LoadMap(const std::string& mapID);
	bool SaveMap();
	void DrawDebug();
	void Clear();
	
	void SwitchMap();
	void FlushMap();

	std::unordered_map<string, MapDataPtr>& GetMapRooms();
	std::unordered_map<string, int> GetMapRoomCount()const;
	std::vector<string> GetMapRoomIDs()const;
	std::vector<Rect> GetMapRoomRects(const std::string& id)const;
	MapDataPtr GetMapData(const std::string& id);


	int GetMinLayer()const;
	int GetMaxLayer()const;
	Size GetSize()const;
	const string& getTitlesetID()const;

	static const int cellSize = 32 * 3;
	static const int maxTryTimes = 50;

	/** 走廊数据 */
	enum HallWayType {
		HallWayUpDown,
		HallWayLeftRight,
		HallWayCenter,


	};

	/** 房间位置数据 */
	struct RoomRect
	{
		Rect rect;
		std::string mapID;
	};

	/** 走廊数据 */
	struct HallwayData
	{
		HallWayType type;
		Point2 pos;
	};
	std::vector<HallwayData> GetHallwayDatas()const;

private:
	void AddMapData(const std::string& name, MapDataPtr mapData, int count);
	bool RandomGenerateMap();
	bool RandomGenerateHallWay(util::Grid<int>& grid, std::vector<RoomRect>& roomRects);
	bool QuickGenerateHallWay(util::Grid<int>& grid, std::vector<RoomRect>& roomRects);
	bool ConnectRegions(util::Grid<int>& grid, const std::vector<int>& regions);
	bool RemoveDeadWay(util::Grid<int>& grid);
	bool HandelMapGrid(util::Grid<int>& grid);

	void AddHallwayData(util::Grid<int>& grid, int col, int row);
private:
	std::string mMapID;
	Point2 mPos;
	Size mSize;
	int mMinLayer, mMaxLayer;
	int mMinRoomCount, mMaxRoomCount;
	int mRoomCount;
	std::string mTitleSetID;

	std::vector<string> mMapRoomIDs;
	std::unordered_map<string, MapDataPtr> mMapRooms;
	std::unordered_map<string, int> mMapRoomCount;
	std::unordered_map<string, std::vector<Rect> > mMapRoomRect;

	// hallway data
	float mWindingPrecent;
	bool mRandomGenerateHallWay;
	int mHallwayIndex;
	std::vector<HallwayData> mHallwaydatas;
	std::unordered_map<HallWayType, MapDataPtr> mHallways;

	// debug data
	std::vector<int> mDebugGrid;
};