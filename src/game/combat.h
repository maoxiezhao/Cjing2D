#pragma once

/**
*	\brief ս������
*
*	��ʱ��֪���Ƿ��ȷ��Ҫ���ģ��
*
*	DoCombat(attacker, victim)
*	���ڹ�����һ�㴥��notifyAttack,�Ҷ��ڲ�ͬ�ı��������ͣ�������ͬ����
*   �����ڱ��������򴥷�notifyTryHurt, ͬʱ����notifyAttack
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