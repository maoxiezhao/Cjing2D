#pragma once

#include"game\enumInfo.h"

/**
*	\brief Entity 类型数据相关描述
*/
enum EntityType
{
	TITLE = 1,		/** 地图块 */
	DYNAMIC_TITLE,  /** 动态地图块 */
	DESTIMATION,	/** 目标点 */
	CAMERA,			/** 相机对象 */
	PLAYRE,			/** 玩家对象 */
	PICKABLE,		/** 可拾取对象 */
	ENEMEY,			/** 敌人 */
	BULLET,			/** 子弹 */
	WEAPON,			/** 武器 */
	BLOCK,			/** 方块 */
	CHEST,			/** 宝箱 */
	TRIGGER,		/** 触发器 */
	UNKNOW,			/** 未知的类型 */
};

// 用于获取key枚举值对应的字符
template<>
struct EnumInfoTraits<EntityType>
{
	static const string prettyName;
	static const EnumInfo<EntityType>::nameType names;
};
