#pragma once

#include "entity\entity.h"

/**
*	\brief Player��ͼ����Ŀ���
*
*	���Entity�������Player�������ͼ�еĳ������꣬һ����ͼ������һ��
*/
class Destination : public Entity
{
public:
	Destination(const std::string& name, const Point2& pos, int layer) :
		Entity(name, "Desination", pos, { 0, 0 }, layer) {}

	virtual EntityType GetEntityType()const
	{
		return EntityType::DESTIMATION;
	}

};