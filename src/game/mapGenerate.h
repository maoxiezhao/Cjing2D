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
*	\brief ��ͼ������ɹ�������
*
*	�ӵ�ͼģ��������ȡ����Ӧ�ĵ�ͼ���䣬Ȼ������Ų��ڵ�ͼ�У�
*	ʹ�ú�ˮ��䷨������ͨ���������Ӹ���ͨ�������䣬�洢���ṩ��
*   Map class����
*
*	TODO �����߼��е���ң���Ҫ�ع� 18/5/19
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

	/** �������� */
	enum HallWayType {
		HallWayUpDown,
		HallWayLeftRight,
		HallWayCenter,


	};

	/** ����λ������ */
	struct RoomRect
	{
		Rect rect;
		std::string mapID;
	};

	/** �������� */
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