#pragma once

#include"movements\movement.h"
#include"utils\vec2.h"

#include<list>

/**
*	\brief ·���ƶ�
*
*	·���ƶ���8pixel��Ϊ��С�ƶ���λ��·��ΪһЩ�з��򼯺�
*/
class PathMovement : public Movement
{
public:
	PathMovement(const std::string& path, int speed, bool loop, bool ingoreObstacles);

	virtual void Update();
	virtual bool IsStarted()const;
	virtual void SetSuspended(bool suspended);
	virtual bool IsFinished()const;
	virtual void Start();

	/** base status */
	void SetSpeed(int speed);
	int GetSpeed()const;
	void SetLoop(bool loop);
	bool IsLoop()const;

	/** path status */
	void SetPath(const std::list<int>& paths);
	void SetPathString(const std::string& pathsString);
	bool IsCurPathStepFinished()const;

	/** lua status */
	virtual const string GetLuaObjectName()const;

private:
	void StartNextPathStep();
	void UpdateCurPathStep();
	void SetNextStepDelay(uint32_t delay);
	uint32_t GetDelayBySpeed(int speed, int dir)const;
	void Restart();

private:
	std::list<int> mSetPaths;
	std::list<int> mCurPaths;
	int mCurPathDir;
	static std::map<int, Point2> pathDirectionShift;

	uint32_t mNextStepDate;
	int mCurSubStep;
	bool mSubStepFinished;
	int32_t mSubStepDelay;

	bool mStopedByObstacle;
	int mSpeed;
	bool mLoop;
};
