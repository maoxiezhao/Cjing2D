#pragma once

/**
*	\brief ����ģ��
*
*	����ģ���ṩ��������Ļ����ӿڣ�ͬʱ�����lua Data
*	�ж�ȡ��Ӧ���������ݣ�������ά�������������������ƽ�
*	����Ŀ��
*/
namespace Quest
{
class QuestManager
{
public:
	~QuestManager() {}

	QuestManager& GetInstance() {}

private:
	QuestManager() {}
};

}