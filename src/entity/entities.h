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
*	\brief Entities管理entitiy
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
	void SetGround(const Ground& ground, int layer, const Rect& rect);
	void SetGround(const Ground& ground, int layer, int cellX, int cellY);
	Ground GetGround(int layer, int cellX, int cellY)const;

	// lua
	LuaContext& GetLuaContext();
	Map& GetMap();
	
private:
	template<typename T>
	using ByLayer = std::map<int, T>;

	// system
	Game& mGame;
	Map& mMap;

	int mMapCellWidth;	/** 地图的最小单元x,y方向个数 */
	int mMapCellHeight;

	// entities
	CameraPtr mCamera;

	std::map<string, EntityPtr>  mEntityNamed;

	EntityList mAllEntities;				// 保存所有entities实体

	EntityTree mEntityTree;					// 用于划分加速,同时真正参与到渲染判定的entity合集

	EntityList mEntityToRemove;				// 移除的entity列表				
			
	ByLayer<EntityVector> mEntityToDraw;	// 绘制entity队列

	ByLayer<std::unique_ptr<TileRegions> > mTileRegions;	// tile绘制层

	ByLayer<std::vector<Ground> > mGrounds;	// 当前地面地形,当前地形以16x16为单位存储

	std::shared_ptr<Player> mPlayer;		// 当前控制对象

};
