#pragma once

#include"common\common.h"
#include"game\map.h"
#include"game\game.h"
#include"utils\quadTree.h"
#include"entity\camera.h"
#include"entity\tileRegions.h"
#include"entity\groundInfo.h"
#include"game\mapData.h"
#include<list>

class Tile;
class LuaContext;
class TileInfo;

using EntityList = std::list<EntityPtr>;
using EntityVector = std::vector<EntityPtr>;
using EntityTree = QuadTree<EntityPtr>;

/**
*	\brief Entities����entitiy
*/
class Entities
{
public:
	Entities(Game&game, Map&map);
	~Entities();

	/** system */
	void Update();
	void Draw();
	void SetSuspended(bool suspended);
	void NotifyMapStarted();
	void ClearRemovedEntites();

	/** entities */
	void InitEntities(const MapData& mapData);
	void InitEntities(const MapData& mapData, const Point2& pos, const std::string& roomName);
	void AddEntity(const EntityPtr& entity);
	void RemoveEntity(Entity& entity);
	CameraPtr GetCamear()const;
	EntityList GetEntities();
	void GetEntitiesInRect(const Rect& rect, EntityVector& entities)const;
	void AddTile(const TileInfo& tileInfo);
	void SetGround(const Ground& ground, int layer, const Rect& rect);
	void SetGround(const Ground& ground, int layer, int cellX, int cellY);
	Ground GetGround(int layer, int cellX, int cellY)const;
	uint32_t GetEntityValueZ(const EntityPtr& entity) const;

	// lua
	LuaContext& GetLuaContext();
	Map& GetMap();

	// notify
	void NotifyEntityRectChanged(Entity& entity);
	
private:
	template<typename T>
	using ByLayer = std::map<int, T>;

	/**
	*	\brief Z���棬�ö�����Ҫ����entity����˳��
	*	��entity����������ͬʱ������zcache��ֵ����
	*	����˳��
	*
	*	zcache��ֵ��entity����˳�����
	*/
	struct ZCache
	{
		ZCache();

		bool AddEntity(EntityPtr entity);
		bool RemoveEntity(EntityPtr entity);
		int GetZ(const EntityPtr& entity)const;

		uint32_t mValueZ;
		ByLayer<std::map<EntityPtr, uint32_t> > mZCache;
	};

	// system
	Game& mGame;
	Map& mMap;

	int mMapCellWidth;	/** ��ͼ����С��Ԫx,y������� */
	int mMapCellHeight;

	// entities
	CameraPtr mCamera;

	std::map<string, EntityPtr>  mEntityNamed;

	EntityList mAllEntities;				// ��������entitiesʵ��

	EntityTree mEntityTree;					// ���ڻ��ּ���,ͬʱ�������뵽��Ⱦ�ж���entity�ϼ�

	EntityList mEntityToRemove;				// �Ƴ���entity�б�				
			
	ByLayer<EntityVector> mEntityToDraw;	// ����entity����

	ByLayer<std::unique_ptr<TileRegions> > mTileRegions;	// tile���Ʋ�

	ByLayer<std::vector<Ground> > mGrounds;	// ��ǰ�������,��ǰ������16x16Ϊ��λ�洢

	std::shared_ptr<Player> mPlayer;		// ��ǰ���ƶ���

	ZCache mZCache;							// ��ǰzֵ����
};
