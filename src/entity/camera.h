#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"common\common.h"
#include"entity\entity.h"

/**
*	\brief 相机对象
*
*	相机对象主要是用来对地图限制位置进行控制，同时提供总体的显示特效
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