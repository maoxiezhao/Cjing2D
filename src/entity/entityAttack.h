#pragma once

/**
*	\brief EntityAttack������Entity��ս����ص����ݺͽӿ�
*/

/**
*	\brief ս��������ʽ
*/
enum class EntityAttack
{
	COMBAT = 1,			/** ��ս */
	BULLET,			    /** �ӵ��˺� */
	BOOMERANG,		    /** ��ը�˺� */
};

/**
*	\brief ս��������ʱ�ķ�ӳ
*/
enum class EntityReactionType
{
	REACTION_IGNORE = 1, /** ���Թ��� */			
	REACTION_HURT,	     /** ���� */
	REACTION_CUSTOM,     /** �ڽű��д��� */
};

