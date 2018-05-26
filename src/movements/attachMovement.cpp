#include "attachMovement.h"
#include "entity\entity.h"

AttachMovement::AttachMovement(Entity * attachEntity, const Point2 & offset, 
	bool isIngroedObstacles) :
	Movement(isIngroedObstacles),
	mAttachEntity(attachEntity),
	mOffset(offset),
	mAttachPoint(attachEntity->GetPos() + offset)
{
}

void AttachMovement::Update()
{
	Movement::Update();

	if (mAttachEntity == nullptr)
		return;

	if (mAttachEntity->IsBeRemoved())
	{
		mAttachEntity = nullptr;
		Stop();
	}

	if (!IsSuspended())
	{
		Point2 newPos = mAttachEntity->GetPos() + mOffset;
		if (newPos != mAttachPoint)
		{
			if (IsIngoredObstacles())
			{
				SetPos(newPos);
			}
			else
			{
				Point2 diff = newPos - GetPos();
				if (!TestCollisionWithObstacles(diff))
				{
					TranslatePos(diff);
				}
				else
				{
					Stop();
					NotifyObstacleReached();
				}
			}
		}
	}
}

const string AttachMovement::GetLuaObjectName() const
{
	return string();
}
