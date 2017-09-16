#include "entities.h"
#include "lua\luaContext.h"

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

Entities::~Entities()
{
	mEntityNamed.clear();
	mEntityToDraw.clear();
	mEntityToRemove.clear();
	mAllEntities.clear();
}

/**
*	\brief ����entity
*/
void Entities::Update()
{
	for (const auto& entity : mAllEntities)
	{
		entity->Update();
	}

	mEntityToDraw.clear();
}

/**
*	\brief ����entity
*/
void Entities::Draw()
{
	// �����Ҫ���Ƶ�entity��������ֻҪentity��
	// �������Χʱ����ӵ������б�
	if (mEntityToDraw.empty())
	{
		for (const auto& entity : mAllEntities)
		{
			int layer = entity->GetLayer();
			mEntityToDraw[layer].push_back(entity);
		}
	}

	// ���ݲ㼶����entity
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		// ����tile

		// ������ͨentity
		auto curLayerEntityToDraw = mEntityToDraw[layer];
		for (const auto& entity : curLayerEntityToDraw)
		{
			entity->Draw();
		}
	}
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
void Entities::InitEntities(const MapData& mapData)
{
	// ���ղ㼶˳�򴴽�entity
	for (int curLayer = mapData.GetMinLayer(); curLayer <= mapData.GetMaxLayer(); curLayer++)
	{
		for (int index = 0; index < mapData.GetEntityCountByLayer(curLayer); index++)
		{
			const EntityData& entityData = mapData.GetEntity(curLayer, index);
			if (!GetLuaContext().CreateEntity(entityData, mMap))
			{
				Debug::Error("Failed to create entity.");
			}
		}
	}
}

/**
*	\biref ���һ���µ�entity
*/
void Entities::AddEntity(const EntityPtr& entity)
{
	if (entity == nullptr)
	{
		return;
	}
	Debug::CheckAssertion(mMap.IsValidLayer(entity->GetLayer()),
		"Invalid entity layer in adding entity.");

	mAllEntities.push_back(entity);
	entity->SetMap(&mMap);
}

void Entities::RemoveEntity(Entity& entity)
{
}

LuaContext & Entities::GetLuaContext()
{
	return mGame.GetLuaContext();
}

Map & Entities::GetMap()
{
	return mMap;
}

