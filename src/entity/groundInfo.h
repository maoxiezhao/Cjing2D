#pragma once

#include"game\enumInfo.h"

/**
*	\brief ground��Ϣ����
*/
enum Ground
{
	GROUND_EMPTY,	/** �յ��� */
	GROUND_WALL	/** ǽ���� */
};

template<>
struct EnumInfoTraits<Ground>
{
	static const string prettyName;
	static const EnumInfo<Ground>::nameType names;
};

/**
*	\brief ����ground����С��Ԫ��С
*/
static int groundCellWidth  = 8;
static int groundCellHeight = 8;
