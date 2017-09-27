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
using EntityTree = QuadTree<Entity>;

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

	/** entities */
	void InitEntities(const MapData& mapData);
	void AddEntity(const EntityPtr& entity);
	void RemoveEntity(Entity& entity);
	CameraPtr GetCamear()const;
	EntityList GetEntities();
	void AddTile(const TileInfo& tileInfo);

	// lua
	LuaContext& GetLuaContext();
	Map& GetMap();
	
private:
	template<typename T>
	using ByLayer = std::map<int, T>;

	// system
	Game& mGame;
	Map& mMap;
	int mMapWidth;
	int mMapHeight;

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

};