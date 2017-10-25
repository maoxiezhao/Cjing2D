#pragma once

#include"movements\movement.h"
#include<list>

/**
*	\brief �������movement
*	����movement�ǰ�����ִ��movements,������ӵ�
*	movement����Ӧ������ǳ���������
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
	std::vector<MovementPtr> mMovementSequence;		/** movement ���� */
	int mCurrMovementIndex;
	MovementPtr mCurrMovement;
	bool mFinished;
	bool mLooped;

};