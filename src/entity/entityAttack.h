#pragma once

/**
*	\brief EntityAttack������Entity��ս����ص����ݺͽӿ�
*/

/**
*	\brief ս��������ʽ
*/
enum class EntityAttack
{
	COMBAT,			/** ��ս */
	BULLET,			/** �ӵ��˺� */
	BOOMERANG,		/** ��ը�˺� */
};

/**
*	\brief ս��������ʱ�ķ�ӳ
*/
enum class EntityReactionType
{
	REACTION_IGNORE, /** ���Թ��� */			
	REACTION_HURT,	 /** ���� */
	REACTION_CUSTOM, /** �ڽű��д��� */
};

