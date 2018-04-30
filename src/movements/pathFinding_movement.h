#pragma once

#include "movements\pathMovement.h"
#include "movements\pathFinding_private.h"
#include "entity\entity.h"

/**
*	\brief 寻路路径移动，通过寻路算法获取路径后按路径移动
*
*	因为仅8方向移动，所以不太需要路径平滑操作
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

	EntityPtr mTargetEntity;	/** 目标Entity */
	Point2 mTargetPoint;		/** 目标位置，和Entity互斥 */
	uint32_t mNextRecomputeDate;/** 下一次计算寻路的时间 */
	bool mRandomPath;			/** 当不存在寻路结果时，是否生成随机路径 */

	std::unique_ptr<PathFindingPrivate::PathFinding> mPathFinding;
};