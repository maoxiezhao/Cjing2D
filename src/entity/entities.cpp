#include "entities.h"
#include "core\video.h"
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

	// ������,Test:��ʱ����Ϊ��Ļ�е㣬��ҰΪ������Ļ
	mCamera = std::make_shared<Camera>();
	mCamera->SetSize(Video::GetScreenSize() / 2);
	mCamera->SetPos({ Video::GetScreenSize().width / 2, Video::GetScreenSize().height / 2 });
	AddEntity(mCamera);
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
			Rect aroundCamera(
				mCamera->GetPos().x - mCamera->GetSize().width,
				mCamera->GetPos().y - mCamera->GetSize().height,
				mCamera->GetSize().width * 2,
				mCamera->GetSize().height * 2);

			if (entity->GetRectBounding().Overlaps(aroundCamera))
			{
				int layer = entity->GetLayer();
				mEntityToDraw[layer].push_back(entity);
			}
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

/**
*	\brief �������
*/
CameraPtr Entities::GetCamear() const
{
	return mCamera;
}

/**
*	\brief ����entities�ļ���
*/
EntityList Entities::GetEntities()
{
	return mAllEntities;
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

