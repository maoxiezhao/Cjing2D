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
	// ��ʼ��ÿ���layer

	// ��ʼ���Ĳ���
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
*	\brief ����entityData����entities
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

