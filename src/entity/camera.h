#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"common\common.h"
#include"entity\entity.h"

/**
*	\brief �������
*
*	���������Ҫ�������Ե�ͼ����λ�ý��п��ƣ�ͬʱ�ṩ�������ʾ��Ч
*	���������entity�����У�posָ����������������꣬sizeΪ���xy����
*	�ϵķ�Χ���ܵķ�ΧӦΪ2����size
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