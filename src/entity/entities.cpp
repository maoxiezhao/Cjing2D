#include "entities.h"
#include "entity\player.h"
#include "core\video.h"
#include "lua\luaContext.h"

Entities::Entities(Game&game, Map&map):
	mGame(game),
	mMap(map),
	mEntityNamed(),
	mAllEntities(),
	mEntityToDraw(),
	mEntityToRemove(),
	mCamera(nullptr),
	mPlayer(game.GetPlayer())
{
	// ��ʼ��ÿ���layer
	mMapCellWidth = mMap.GetWidth() / groundCellWidth;
	mMapCellHeight = mMap.GetHeight() / groundCellHeight;
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		TileRegions* tileRegion = new TileRegions(map, layer);
		mTileRegions.emplace(layer, std::unique_ptr<TileRegions>(tileRegion));

		mGrounds[layer].resize(mMapCellWidth*mMapCellHeight, Ground::GROUND_EMPTY);
	}

	// ��ʼ���Ĳ�������entity

	// ������,Test:��ʱ����Ϊ��Ļ�е㣬��ҰΪ������Ļ
	mCamera = std::make_shared<Camera>(map);
	mCamera->SetSize(Video::GetScreenSize() / 2);
	mCamera->SetPos({ Video::GetScreenSize().width / 2, Video::GetScreenSize().height / 2 });
	mCamera->TracingEntity(*mPlayer);
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
	// update player
	mPlayer->Update();

	// update all entites
	for (const auto& entity : mAllEntities)
	{
		entity->Update();
	}

	mCamera->Update();
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
		// Ӧ��4�����л�ȡ���󼯺�

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

		// ��entityToDraw��������
		for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++) 
		{
			std::sort(mEntityToDraw[layer].begin(), mEntityToDraw[layer].end(), [](EntityPtr& entity1, EntityPtr& entity2) {
				// layer compare
				if (entity1->GetLayer() < entity1->GetLayer())
				{
					return true;
				}
				else if (entity1->GetLayer() > entity1->GetLayer())
				{
					return false;
				}
				else
				{	// ��layer��ͬʱ�������y����Ƚ�
					if (!entity1->IsDrawOnYOrder() && entity2->IsDrawOnYOrder())
					{
						return true;
					}
					else if (entity1->IsDrawOnYOrder() && !entity2->IsDrawOnYOrder())
					{
						return false;
					}
					else if (entity1->IsDrawOnYOrder() && entity2->IsDrawOnYOrder())
					{
						if (entity1->GetPos().y < entity2->GetPos().y)
						{
							return true;
						}
						else if (entity1->GetPos().y > entity2->GetPos().y)
						{
							return false;
						}
					}		
					// ��layer��ͬ��y����Ҳ��ͬʱ
					return true;
				}
			});
		}
	}

	// ���ݲ㼶����entity
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		// ����tile
		mTileRegions[layer]->Draw();

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
*	\brief ��Ӧ��ͼ�����ɹ�
*/
void Entities::NotifyMapStarted()
{
	// ����ÿ��tileRegions��tile
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		mTileRegions[layer]->Build();
	}

	// entity��Ӧmap startd
	for (const auto& entity : mAllEntities)
	{
		entity->NotifyMapStarted();
	}
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
*	\brief ���tileͼ��
*/
void Entities::AddTile(const TileInfo & tileInfo)
{
	Debug::CheckAssertion(mMap.IsValidLayer(tileInfo.mLayer),"Invalid tile layer.");

	mTileRegions[tileInfo.mLayer]->AddTile(tileInfo);

	// add ground
	int tileLayer = tileInfo.mLayer;
	const Size tileSize = tileInfo.mSize;
	const Point2 tilePos = tileInfo.mPos;
	const Ground ground = tileInfo.mGround;

	switch (ground)
	{
	case Ground::GROUND_WALL:
		SetGround(ground, tileLayer, Rect(tilePos, tileSize));
		break;

	case Ground::GROUND_EMPTY:
		// empty do nothing.
		break;
	}

}

/**
*	\brief ���õ������
*	\param rect ���õķ�Χ
*/
void Entities::SetGround(const Ground & ground, int layer, const Rect & rect)
{
	int cellBeginX = rect.x / groundCellWidth;
	int cellEndX = (rect.x + rect.width) / groundCellWidth;
	int cellBeginY = rect.y / groundCellHeight;
	int cellEndY = (rect.y + rect.height) / groundCellHeight;

	for (int cellY = cellBeginY; cellY < cellEndY; cellY++)
	{
		if (cellY < 0 || cellY >= mMapCellHeight)
		{
			continue;
		}
		for (int cellX = cellBeginX; cellX < cellEndX; cellX++)
		{
			SetGround(ground, layer, cellX, cellY);
		}
	}
}

/**
*	\brief ���õ������
*	\param cellX x����ĵ�n��cell
*	\param cellY y����ĵ�n��cell
*/
void Entities::SetGround(const Ground & ground, int layer, int cellX, int cellY)
{
	if (cellX >= 0 && cellX < mMapCellWidth && 
		cellY >= 0 && cellY < mMapCellHeight)
	{
		mGrounds[layer][cellY*mMapCellWidth + cellX] = ground;
	}
}


/**
*	\brief ��ȡָ��λ��ground��inline
*/
Ground Entities::GetGround(int layer, int cellX, int cellY)const
{
	if (cellX >= 0 && cellX < mMapCellWidth &&
		cellY >= 0 && cellY < mMapCellHeight)
	{
		return mGrounds.at(layer).at(cellY*mMapCellWidth + cellX);
	}
	return Ground::GROUND_EMPTY;
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

	//if (entity->GetEntityType() != EntityType::PLAYRE)
	//{
		mAllEntities.push_back(entity);
		entity->SetMap(&mMap);
	//}
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

