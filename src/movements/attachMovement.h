#pragma once

#include"common\common.h"
#include"movements\movement.h"
#include"utils\vec2.h"

/**
*	\brief ������ĳ��Entity��������˶�
*/
class AttachMovement : public Movement
{
public:
	AttachMovement(Entity* attachEntity, const Point2& offset, bool isIngroedObstacles);

	virtual void Update();
	virtual const string GetLuaObjectName()const;

private:
	Entity* mAttachEntity;
	Point2 mOffset;
	Point2 mAttachPoint;
};