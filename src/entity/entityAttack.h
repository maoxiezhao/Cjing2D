#pragma once

/**
*	\brief EntityAttack定义了Entity间战斗相关的数据和接口
*/

/**
*	\brief 战斗攻击方式
*/
enum class EntityAttack
{
	COMBAT,			/** 近战 */
	BULLET,			/** 子弹伤害 */
	BOOMERANG,		/** 爆炸伤害 */
};

/**
*	\brief 战斗被攻击时的反映
*/
enum class EntityReactionType
{
	REACTION_IGNORE, /** 忽略攻击 */			
	REACTION_HURT,	 /** 受伤 */
	REACTION_CUSTOM, /** 在脚本中处理 */
};

