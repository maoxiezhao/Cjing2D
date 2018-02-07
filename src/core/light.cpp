#include "light.h"

Light::Light(const Vec3i & pos, int radius, Color4B color):
	mPos(pos),
	mRadius(radius),
	mColor(color)
{
}

Light::~Light()
{
}

void Light::Update()
{
}

void Light::Render()
{
}

void Light::DebugRender()
{
}

/**
*	\brief ����Դ���ݴ��͵�program
*/
void Light::SetLightToProgram(GLProgramState& programState, int index)
{
	
}


AreaLight::AreaLight(const Vec3i & pos, int radius, Color4B color) :
	Light(pos, radius, color)
{
}

void AreaLight::Update()
{
}

void AreaLight::Render()
{
}

void AreaLight::SetLightToProgram(GLProgramState & programState, int index)
{
}

PointLight::PointLight(const Vec3i & pos, int radius, Color4B color):
	Light(pos, radius, color)
{
}

void PointLight::Update()
{
}

void PointLight::Render()
{
}

/**
*	\brief �����Դ���ݴ��͵�program
*
*	���δ��͵��Դλ�ã��������꣩�����Դ�뾶�����Դ��ɫ
*/
void PointLight::SetLightToProgram(GLProgramState & programState, int index)
{
	const std::string lightStr = "lights[" + std::to_string(index) + "].";
	programState.SetUniform3f(lightStr + "Position", GetPos().ToVecE<float>());
	programState.SetUniform1f(lightStr + "Radius", (float)GetRadius());
	programState.SetUniform3f(lightStr + "Color",  GetColor().GetColorRGB3F());
}

void PointLight::Clear()
{
	mObstacles.clear();
	mLightMesh.clear();
}

void PointLight::PushObstacle(const Rect & rect)
{
	mObstacles.push_back(rect);
}

/**
*	\brief ���������������
*
*	����Ⱦ�׶ε��øú���������տɼ����������񣬶�
*	������ÿ��rect����������߼��
*/
void PointLight::CalucateLightMesh()
{

}
