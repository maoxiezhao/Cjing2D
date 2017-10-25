#ifndef _TARGET_MOVEMENT_H_
#define _TARGET_MOVEMENT_H_

#include"movements\straightMovement.h"
#include"entity\entity.h"
#include"utils\point.h"

/**
*	\brief 线性目标移动
*/
class TargetMovement : public StraightMovement
{
public:
	TargetMovement(
		const EntityPtr& targetEntity,
		const Point2& target,
		int speed,
		bool ingoreObstacles
	);

	virtual void SetDrawable(Drawable* drawable);

	void SetTarget(const EntityPtr& entity, const Point2& pos);
	Point2 GetTargetPos()const;

	void SetMovingSpeed(int movingSpeed);
	int GetMovingSpeed()const;

	virtual void Start();
	virtual void Update();
	virtual bool IsFinished()const;
	virtual const string GetLuaObjectName()const;

private:
	void RefreshMovment();
	
	Point2 mTargetPos;
	Point2 mTargetOffset;
	EntityPtr mTargetEntity;	// 跟随的目标实体，可为null

	int mMovingSpeed;
	int mSignX;					// 用于检测是否发生运动超出现象
	int mSignY;
	uint32_t mNextRefreshMovmentDate;

	bool mFinished;
};


#endif