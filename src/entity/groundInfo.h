#pragma once

#include"game\enumInfo.h"

/**
*	\brief ground信息定义
*/
enum Ground
{
	GROUND_EMPTY,	/** 空地形 */
	GROUND_WALL	/** 墙地形 */
};

template<>
struct EnumInfoTraits<Ground>
{
	static const string prettyName;
	static const EnumInfo<Ground>::nameType names;
};

/**
*	\brief 处理ground的最小单元大小
*/
static int groundCellWidth  = 8;
static int groundCellHeight = 8;
