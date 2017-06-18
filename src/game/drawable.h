#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include"common\common.h"
#include"movements\movement.h"
#include"utils\point.h"
#include"lua\luaObject.h"

/**
*	\brief 提供位置属性的基本绘制单元
*/
class Drawable : public LuaObject
{
public:
	~Drawable();

	virtual void Update();
	virtual void Draw() = 0;

	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	float GetRotated()const;
	void SetRotated(float angle);
	float GetGlobalOrder()const;
	void SetGlobalOrder(float order);

	virtual void SetSuspended(bool suspended);
	bool IsSuspended()const;

	// lua
	virtual const string GetLuaObjectName()const ;

protected:
	Drawable();

	Point2 mPos;
	float mRotate;
	float mGlobalZOrder;	// 全局次序，决定了渲染时的绘制次序
	float mLocalZOrder;		// 局部测序，在作为子节点时的操作次序
	bool mSuspended;		

	std::vector<MovementPtr> mMovements;

};



#endif