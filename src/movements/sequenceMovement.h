#pragma once

#include"movements\movement.h"
#include<list>

/**
*	\brief 序列组合movement
*	序列movement是按序列执行movements,对于添加的
*	movement本身应该满足非持续性条件
*/
class SequenceMovement : public Movement
{
public:
	SequenceMovement(bool isIngroedObstacles, bool looped = false);
	~SequenceMovement();

	virtual void Update();
	bool IsSuspended()const;
	virtual void SetSuspended(bool suspended);
	virtual void SetDrawable(Drawable* drawable);
	virtual void SetEntity(Entity* entity);

	// move
	void SetFinished(bool finished);
	virtual bool IsFinished()const;
	virtual void Start();
	virtual void Stop();

	// movements 
	bool HasMovement(const MovementPtr& movement);
	void AddMovement(MovementPtr movement);
	MovementPtr GetNextMovement();
	virtual const string GetLuaObjectName()const;

private:
	std::vector<MovementPtr> mMovementSequence;		/** movement 序列 */
	int mCurrMovementIndex;
	MovementPtr mCurrMovement;
	bool mFinished;
	bool mLooped;

};