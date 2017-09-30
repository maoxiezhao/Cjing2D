#include "groundInfo.h"

const string  EnumInfoTraits<Ground>::prettyName = "Ground";
const EnumInfo<Ground>::nameType EnumInfoTraits<Ground>::names = {
	{GROUND_EMPTY , ""},
	{GROUND_WALL, "Wall"}
};