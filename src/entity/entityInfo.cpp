#include"entity\entityInfo.h"

const string EnumInfoTraits<EntityType>::prettyName = "entity type";
const EnumInfo<EntityType>::nameType EnumInfoTraits<EntityType>::names =
{
	{ TITLE,"title"},
	{ DYNAMIC_TITLE,"dynamicTitle"},
	{ DESTIMATION, "Destimation"}
};