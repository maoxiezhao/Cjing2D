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
	// 初始化每层的layer
	int mapWidth16 = mMap.GetWidth() / groundCellWidth;
	int mapHeight16 = mMap.GetHeight() / groundCellHeight;
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		TileRegions* tileRegion = new TileRegions(map, layer);
		mTileRegions.emplace(layer, std::unique_ptr<TileRegions>(tileRegion));

		mGrounds[layer].resize(mapWidth16*mapHeight16, Ground::GROUND_EMPTY);
	}

	// 初始化四叉树管理entity

	// 添加相机,Test:临时设置为屏幕中点，视野为整个屏幕
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
*	\brief 更新entity
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
*	\brief 绘制entity
*/
void Entities::Draw()
{
	// 添加需要绘制的entity，这里是只要entity在
	// 在相机范围时才添加到绘制列表
	if (mEntityToDraw.empty())
	{
		// 应从4叉数中获取对象集合

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

		// 对entityToDraw进行排序
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
				{	// 当layer相同时，则根据y坐标比较
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
					// 当layer相同，y坐标也相同时
					return true;
				}
			});
		}
	}

	// 根据层级绘制entity
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		// 绘制tile
		mTileRegions[layer]->Draw();

		// 绘制普通entity
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
*	\brief 响应地图创建成功
*/
void Entities::NotifyMapStarted()
{
	// 创建每层tileRegions的tile
	for (int layer = mMap.GetMinLayer(); layer <= mMap.GetMaxLayer(); layer++)
	{
		mTileRegions[layer]->Build();
	}

	// entity响应map startd
	for (const auto& entity : mAllEntities)
	{
		entity->NotifyMapStarted();
	}
}

/**
*	\brief 返回相机
*/
CameraPtr Entities::GetCamear() const
{
	return mCamera;
}

/**
*	\brief 返回entities的集合
*/
EntityList Entities::GetEntities()
{
	return mAllEntities;
}

/**
*	\brief 添加tile图块
*/
void Entities::AddTile(const TileInfo & tileInfo)
{
	Debug::CheckAssertion(mMap.IsValidLayer(tileInfo.mLayer),"Invalid tile layer.");

	mTileRegions[tileInfo.mLayer]->AddTile(tileInfo);
}

/**
*	\brief 根据entityData创建entities
*/
void Entities::InitEntities(const MapData& mapData)
{
	// 按照层级顺序创建entity
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
*	\biref 添加一个新的entity
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

