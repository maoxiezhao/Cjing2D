#pragma once

/**
*	\brief 战斗控制
*
*	暂时不知道是否的确需要这个模块
*
*	DoCombat(attacker, victim)
*	对于攻击者一般触发notifyAttack,且对于不同的被攻击类型，做出不同触发
*   而对于被攻击者则触发notifyTryHurt, 同时传入notifyAttack
*/

enum class AttackStyle
{
	ATTACK_SWORD,
	ATTACK_BULLET,
	ATTACK_SCRIPT,
};

enum class AttackReactionStyle
{
	REACTION_HURT,
	REACTION_PUSHED,
	REACTION_IMMOBILIZED,
	REACTION_IGNORED,
};