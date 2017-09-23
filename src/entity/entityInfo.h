#pragma once

#include"game\enumInfo.h"

/**
*	\brief Entity ���������������
*/
enum EntityType
{
	TITLE,			/** ��ͼ�� */
	DYNAMIC_TITLE,  /** ��̬��ͼ�� */
	DESTIMATION,	/** Ŀ��� */
	CAMERA,			/** ������� */
	PLAYRE,			/** ��Ҷ��� */
	UNKNOW,			/** δ֪������ */
};

// ���ڻ�ȡkeyö��ֵ��Ӧ���ַ�
template<>
struct EnumInfoTraits<EntityType>
{
	static const string prettyName;
	static const EnumInfo<EntityType>::nameType names;
};
