#include "entity\tileRegions.h"
#include "entity\camera.h"
#include "game\map.h"

namespace {
	const Size tileGridCellSize = {256, 256};
}

TileRegions::TileRegions(Map & map, int layer) :
	mMap(map),
	mLayer(layer),
	mTiles(map.GetSize(), tileGridCellSize)
{
}

/**
*	\brief 添加tile Info信息
*/
void TileRegions::AddTile(const TileInfo & tileInfo)
{
	Debug::CheckAssertion(tileInfo.mLayer == mLayer,
		"Adding tile's layer is not same as regions's layer.");

	mTileInfos.push_back(tileInfo);
}

/**
*	\brief 创建tile region对象
*/
void TileRegions::Build()
{
	for (auto tileInfo : mTileInfos)
	{
		TilePtr tile = std::make_shared<Tile>(tileInfo);
		tile->SetMap(&mMap);
		tile->Build();
		mTiles.Add(tile, tile->GetRectBounding());
	}
	mTileInfos.clear();
}

void TileRegions::Draw()
{
	// 根据当前相机范围选择范围内的tile绘制
	auto camera = mMap.GetCamera();
	Rect aroundCamera(
		camera->GetPos().x - camera->GetSize().width,
		camera->GetPos().y - camera->GetSize().height,
		camera->GetSize().width * 2,
		camera->GetSize().height * 2);

	auto& tiles = mTiles.GetElements(aroundCamera);
	for (auto tile : tiles)
	{
		tile->Draw();
	}
}
