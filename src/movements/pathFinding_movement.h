#pragma once

#include "movements\pathMovement.h"
#include "movements\pathFinding_private.h"
#include "entity\entity.h"

/**
*	\brief Ѱ··���ƶ���ͨ��Ѱ·�㷨��ȡ·����·���ƶ�
*
*	��Ϊ��8�����ƶ������Բ�̫��Ҫ·��ƽ������
*/
class PathFindingMovement : public PathMovement
{
public:
	PathFindingMovement(int speed, bool ingoreObstacle);

	virtual void Update();
	virtual bool IsFinished()const;
	
	void SetTarget(EntityPtr targetEntity);
	void SetTarget(const Point2& targetPos, bool clearEntity = false);
	void NotifyPathFindingFinished();

	/** lua status */
	virtual const string GetLuaObjectName()const;

private:
	void RecomputePath();
	std::string CreateRandomPath();

	EntityPtr mTargetEntity;	/** Ŀ��Entity */
	Point2 mTargetPoint;		/** Ŀ��λ�ã���Entity���� */
	uint32_t mNextRecomputeDate;/** ��һ�μ���Ѱ·��ʱ�� */
	bool mRandomPath;			/** ��������Ѱ·���ʱ���Ƿ��������·�� */

	std::unique_ptr<PathFindingPrivate::PathFinding> mPathFinding;
};