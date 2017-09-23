#include"camera.h"

Camera::Camera()
{
}

void Camera::Update()
{
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
