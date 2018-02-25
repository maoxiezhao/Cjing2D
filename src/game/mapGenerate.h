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
	
	void AddRoomInfo(const std::string& roomID, MapRoomInfo roomInfo);
	const std::map<string, MapRoomInfo>& GetRoomInfos()const { return mRoomInfos; }

	//std::string GetMapID()const { return mMapID; }
	//Point2 GetPos()const { return mPos; }
	//Size GetSize()const { return mSize; }
	//int GetMinLayer()const { return mMinLayer; }
	//int GetMaxLayer()const { return mMaxLayer; }
	//bool IsRandomGenerate()const { return mRandomGenerate; }
	
private:
	string mMapID;
	Point2 mPos;
	Size mSize;
	int mMinLayer, mMaxLayer;
	int mMinRooms, mMaxRooms;
	bool mRandomGenerate;
	std::map<string, MapRoomInfo> mRoomInfos;
};

/**
*	\brief 地图随机生成功能试做
*
*	从地图模板库中随机取出对应的地图房间，然后随机排布在地图中，
*	使用洪水填充法，绘制通道，再链接各个通道、房间，存储并提供给
*   Map class管理
*
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

	const std::unordered_map<string, MapDataPtr>& GetMapRooms()const;
	std::unordered_map<string, int> GetMapRoomCount()const;
	std::vector<string> GetMapRoomIDs()const;

	static const int cellSize = 32;
	static const int maxTryTimes = 50;

private:
	void AddMapData(const std::string& name, MapDataPtr mapData, int count);
	bool RandomGenerateMap();
	bool RandomGenerateHallWay();

private:
	std::string mMapID;
	Point2 mPos;
	Size mSize;
	int mMinLayer, mMaxLayer;
	int mRoomCount;

	std::vector<string> mMapRoomIDs;
	std::unordered_map<string, MapDataPtr> mMapRooms;
	std::unordered_map<string, int> mMapRoomCount;
	std::unordered_map<string, std::vector<Rect> > mMapRoomRect;

};