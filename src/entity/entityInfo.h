#pragma once

#include"game\enumInfo.h"

/**
*	\brief Entity ���������������
*/
enum EntityType
{
	TITLE = 1,		/** ��ͼ�� */
	DYNAMIC_TITLE,  /** ��̬��ͼ�� */
	DESTIMATION,	/** Ŀ��� */
	CAMERA,			/** ������� */
	PLAYRE,			/** ��Ҷ��� */
	PICKABLE,		/** ��ʰȡ���� */
	ENEMEY,			/** ���� */
	BULLET,			/** �ӵ� */
	WEAPON,			/** ���� */
	BLOCK,			/** ���� */
	CHEST,			/** ���� */
	TRIGGER,		/** ������ */
	UNKNOW,			/** δ֪������ */
};

// ���ڻ�ȡkeyö��ֵ��Ӧ���ַ�
template<>
struct EnumInfoTraits<EntityType>
{
	static const string prettyName;
	static const EnumInfo<EntityType>::nameType names;
};
