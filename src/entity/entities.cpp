#include "entities.h"

Entities::Entities(Game&game, Map&map):
	mGame(game),
	mMap(map),
	mEntityNamed(),
	mAllEntities(),
	mEntityToDraw(),
	mEntityToRemove(),
	mCamera(nullptr)
{
	// 初始化每层的layer

	// 初始化四叉树
}

void Entities::Update()
{
}

void Entities::Draw()
{
}

void Entities::SetSuspended(bool suspended)
{
}

CameraPtr Entities::GetCamear() const
{
	return CameraPtr();
}

EntityVector Entities::GetEntities()
{
	return EntityVector();
}

/**
*	\brief 根据entityData创建entities
*/
void Entities::InitEntities(const MapData& entityData)
{

}

void Entities::AddEntity(const EntityPtr& entity)
{
}

void Entities::RemoveEntity(Entity& entity)
{
}

