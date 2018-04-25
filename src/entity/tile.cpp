#include "tile.h"
#include "game\map.h"
#include "entity\tileset.h"

Tile::Tile(const TileInfo& tileInfo):
	Entity("", "", tileInfo.mPos, tileInfo.mSize, tileInfo.mLayer),
	mPatternID(tileInfo.mPatternID),
	mPattern(*const_cast<TilePattern*>(tileInfo.mPattern)),
	mTileSprite(nullptr)
{
}

void Tile::Update()
{

}

void Tile::Draw()
{
	Debug::CheckAssertion(mTileSprite != nullptr, "The tile without tile sprite.");

	mTileSprite->SetTextureRect(mPattern.GetTextureRect(), true);
	mTileSprite->SetPos(GetPos());
	mTileSprite->SetSize(GetSize());

	GetMap().DrawOnMap(*mTileSprite);
}

void Tile::Build()
{
	//Entity::Initalized(); // 非主动调用

	auto tilesetImage = GetMap().GetTileset().GetTileImage();
	if (tilesetImage == nullptr)
	{
		Debug::Error("The tileset's image is not exists.");
	}
	mTileSprite = std::make_shared<Sprite>(tilesetImage->GetTexture());
	mTileSprite->SetDeferredDraw(true);
}

EntityType Tile::GetEntityType() const
{
	return EntityType::TITLE;
}
