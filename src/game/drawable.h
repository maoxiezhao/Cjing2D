#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include"common\common.h"
#include"core\light.h"
#include"utils\point.h"
#include"lua\luaObject.h"

class Movement;

/**
*	\brief �ṩλ�����ԵĻ������Ƶ�Ԫ
*/
class Drawable : public LuaObject
{
public:
	~Drawable();

	virtual void Update();
	virtual void Draw();
	virtual void Draw(const Point2& pos);

	// status
	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	float GetRotated()const;
	void SetRotated(float angle);
	float GetGlobalOrder()const;
	void SetGlobalOrder(float order);
	virtual void SetSuspended(bool suspended);
	bool IsSuspended()const;
	void Translate(const Point2& offset);

	// movement
	void StopMovement();
	void StartMovement(const std::shared_ptr<Movement>& movement);
	const std::shared_ptr<Movement>& GetMovement();

	// lua
	virtual const string GetLuaObjectName()const ;

protected:
	Drawable();

	Point2 mPos;
	float mRotate;
	float mGlobalZOrder;	// ȫ�ִ��򣬾�������Ⱦʱ�Ļ��ƴ���
	float mLocalZOrder;		// �ֲ���������Ϊ�ӽڵ�ʱ�Ĳ�������
	bool mSuspended;		

	std::shared_ptr<Movement> mMovement;

	std::vector<std::shared_ptr<Movement> > mMovements;		// ��ǰ���ص��˶����������ʱ��ʹ��

};

using DrawablePtr = std::shared_ptr<Drawable>;

#endif