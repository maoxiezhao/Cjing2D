#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include"common\common.h"
#include"movements\movement.h"
#include"utils\point.h"

/**
*	\brief 提供位置属性的基本绘制单元
*/
class Drawable
{
public:
	virtual ~Drawable();

	virtual void Update();
	virtual void Draw() = 0;

	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	float GetRotated()const;
	void SetRotated(float angle);
	float GetGlobalOrder()const;
	void SetGlobalOrder(float order);

protected:
	Drawable();

	Point2 mPos;
	float mRotate;
	float mGlobalZOrder;
	bool mSuspended;

	std::vector<MovementPtr> mMovements;

};



#endif