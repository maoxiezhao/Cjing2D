#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"common\common.h"
#include"entity\entity.h"

/**
*	\brief 相机对象
*
*	相机对象主要是用来对地图限制位置进行控制，同时提供总体的显示特效
*	其中相机的entity属性中：pos指的是相机的中心坐标，size为相机xy方向
*	上的范围，总的范围应为2倍的size
*/
class Camera : public Entity
{
public:
	Camera(Map& map);

	virtual void Update();
	virtual void Draw();

	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	Size GetViewSize()const;
	Point2 GetLeftTopPos()const;
	void SetLeftTopPos(const Point2& pos);
	void AdaptToMapBound();

	void TracingEntity(Entity& entity);

};

using CameraPtr = std::shared_ptr<Camera>;

#endif