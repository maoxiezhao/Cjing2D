#pragma once

#include"common\common.h"
#include"game\map.h"
#include"game\game.h"
#include"utils\quadTree.h"
#include"entity\camera.h"
#include"game\mapData.h"
#include<list>

class Ground;
class Tile;

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

	// system
	void Update();
	void Draw();
	void SetSuspended(bool suspended);

	CameraPtr GetCamear()const;
	EntityVector GetEntities();

	// entities
	void InitEntities(const MapData& entityData);
	void AddEntity(const EntityPtr& entity);
	void RemoveEntity(Entity& entity);

	// notify

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

	EntityTree mEntityTree;					// ���ڻ��ּ���

	EntityList mEntityToRemove;				

	ByLayer<EntityVector> mEntityToDraw;	

};