#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"common\common.h"
#include"entity\entity.h"

/**
*	\brief �������
*
*	���������Ҫ�������Ե�ͼ����λ�ý��п��ƣ�ͬʱ�ṩ�������ʾ��Ч
*/
class Camera : public Entity
{
public:
	Camera();

	virtual void Update();
	virtual void Draw();

	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;
private:


};

using CameraPtr = std::shared_ptr<Camera>;

#endif