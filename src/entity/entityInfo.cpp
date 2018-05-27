#include"entity\entityInfo.h"

const string EnumInfoTraits<EntityType>::prettyName = "entity type";
const EnumInfo<EntityType>::nameType EnumInfoTraits<EntityType>::names =
{
	{ TITLE,"tile"},
	{ DYNAMIC_TITLE,"dynamicTitle"},
	{ DESTIMATION, "destination"},
	{ CAMERA, "camera"},
	{ PLAYRE, "player"},
	{ PICKABLE, "pickable"},
	{ ENEMEY, "enemy"},
	{ BULLET, "bullet"},
	{ WEAPON, "weapon" },
	{ BLOCK, "block" },
	{ CHEST, "chest" },
};