#pragma once

#include"game\mapData.h"
#include"utils\grid.h"
#include<unordered_map>

/**
*	\brief �����ͼ����
*/
/**
*	\brief MapGenerateData
*
*	MapGenerateData�����˵�ǰ��ͼ�������Ϣ��ͬʱ�����˵�ǰ��ͼ�����
*	������Ϣ��mapData)
*/
class MapGenerateData : public LuaData
{
public:
	MapGenerateData() = default;

	virtual bool ImportFromLua(lua_State*l);
	virtual bool ExportToLua(lua_State*l);
	static int LuaMapGenerateData(lua_State*l);

	/**
	*	\brief ��ͼ������Ϣ
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
*	\brief ��ͼ������ɹ�������
*
*	�ӵ�ͼģ��������ȡ����Ӧ�ĵ�ͼ���䣬Ȼ������Ų��ڵ�ͼ�У�
*	ʹ�ú�ˮ��䷨������ͨ���������Ӹ���ͨ�������䣬�洢���ṩ��
*   Map class����
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

	static const int cellSize = 32 * 3;
	static const int maxTryTimes = 50;

	enum HallWayType {
		HallWayUp,
		HallWayDown,
		HallWayLeft,
		HallWayRight,
	};

private:
	void AddMapData(const std::string& name, MapDataPtr mapData, int count);
	bool RandomGenerateMap();
	bool RandomGenerateHallWay(util::Grid<int>& grid);
	bool QuickGenerateHallWay(util::Grid<int>& grid);
	bool ConnectRegions(util::Grid<int>& grid, const std::vector<int>& regions);
	bool RemoveDeadWay(util::Grid<int>& grid);
private:
	std::string mMapID;
	Point2 mPos;
	Size mSize;
	int mMinLayer, mMaxLayer;
	int mRoomCount;
	bool mRandomGenerateHallWay;

	std::vector<string> mMapRoomIDs;
	std::unordered_map<string, MapDataPtr> mMapRooms;
	std::unordered_map<string, int> mMapRoomCount;
	std::unordered_map<string, std::vector<Rect> > mMapRoomRect;

	// hallway data
	float mWindingPrecent;

	// debug data
	std::vector<int> mDebugGrid;
};