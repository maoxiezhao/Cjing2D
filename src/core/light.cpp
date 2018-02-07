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
*	\brief 将光源数据传送到program
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
*	\brief 将点光源数据传送到program
*
*	依次传送点光源位置（世界坐标），点光源半径，点光源颜色
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
*	\brief 计算光照区域网格
*
*	在渲染阶段调用该函数计算光照可见性区域网格，对
*	区域内每个rect顶点进行射线检测
*/
void PointLight::CalucateLightMesh()
{

}
