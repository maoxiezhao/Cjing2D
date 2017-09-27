#include"camera.h"
#include"game\map.h"
#include"core\video.h"
#include"entity\entityState.h"

namespace {
	/**
	*	\brief �������״̬
	*
	*	��״̬ʹ��ǰ��������λ��ʼ���趨Ϊ���ٵ�entity
	*/
	class TracingState : public EntityState
	{
	public:
		TracingState(Camera& camera, Entity& tracingEntity);

		virtual void Update();
	private:
		Entity& mTracingEntity;
	};

	TracingState::TracingState(Camera& camera, Entity & tracingEntity):
		EntityState(camera),
		mTracingEntity(tracingEntity)
	{
	}

	void TracingState::Update()
	{
		Camera& camera = *dynamic_cast<Camera*>(&GetEntity());
		Point2 centerPos = mTracingEntity.GetCenterPos();
		camera.SetPos(centerPos);
	}

}

Camera::Camera(Map& map):
	Entity("", Point2(0, 0), Video::GetScreenSize(), 0)
{
	SetMap(&map);
}

void Camera::Update()
{
	Entity::Update();

	AdaptToMapBound();
}

void Camera::Draw()
{
}

EntityType Camera::GetEntityType() const
{
	return EntityType::CAMERA;
}

const string Camera::GetLuaObjectName() const
{
	return string();
}

/**
*	\brief ��ȡ�ܵ���Ұ��С
*/
Size Camera::GetViewSize() const
{
	return GetSize() * 2;
}

/**
*	\brief ��ȡ���Ͻ�����
*/
Point2 Camera::GetLeftTopPos() const
{
	return Point2(GetPos().x - GetSize().width,
		GetPos().y - GetSize().height);
}

/**
*	\brief �������Ͻ�����
*/
void Camera::SetLeftTopPos(const Point2 & pos)
{
	SetPos({ pos.x + GetSize().width,
		pos.y + GetSize().height });
}

/**
*	\brief ����camera�Ĵ�С��Ӧ��ͼ��С
*/
void Camera::AdaptToMapBound()
{
	Size mapSize = GetMap().GetSize();
	Size viewSize = GetViewSize();
	Point2 newCamearPos;

	// ��������������귽��
	auto adaptSizeAxis = [](int viewSize, int mapSize, int camPos) ->int
	{
		if (mapSize < viewSize)
		{
			return (mapSize - viewSize) / 2;
		}
		else
		{
			return std::min(std::max(camPos, 0), (mapSize - viewSize));
		}
	};
	newCamearPos.x = adaptSizeAxis(viewSize.width,  mapSize.width,  GetLeftTopPos().x);
	newCamearPos.y = adaptSizeAxis(viewSize.height, mapSize.height, GetLeftTopPos().y);
	SetLeftTopPos(newCamearPos);
}

/**
*	\brief �������Ϊ׷��entity�ƶ�
*/
void Camera::TracingEntity(Entity & entity)
{
	SetState(std::make_shared<TracingState>(*this, entity));
}
