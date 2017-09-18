#include"entity\entityInfo.h"

const string EnumInfoTraits<EntityType>::prettyName = "entity type";
const EnumInfo<EntityType>::nameType EnumInfoTraits<EntityType>::names =
{
	{ TITLE,"tile"},
	{ DYNAMIC_TITLE,"dynamicTitle"},
	{ DESTIMATION, "destimation"},
	{ CAMERA, "camera"}
};