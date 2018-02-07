#pragma once

#include"common\common.h"
#include"core\types.h"
#include"core\vertexArray.h"
#include"core\glProgramState.h"
#include"core\texture.h"
#include"utils\matrix4.h"
#include"utils\point.h"
#include"utils\rectangle.h"

#include<memory>

/**
*	\brief 光照光源相关
*/
class Light
{
public:
	Light(const Vec3i& pos, int radius, Color4B color);
	~Light();

	virtual void Update();
	virtual void Render();
	virtual void DebugRender();
	virtual void SetLightToProgram(GLProgramState& programState, int index);

	Vec3i GetPos()const { return mPos; }
	int GetRadius()const { return mRadius; }
	Color4B GetColor()const { return mColor; }

protected:
	Vec3i mPos;
	int mRadius;
	Color4B mColor;
};

using LightPtr = std::shared_ptr<Light>;

/**
*	\brief 区域光照
*/
class AreaLight : public Light
{
public:
	AreaLight(const Vec3i& pos, int radius, Color4B color);

	virtual void Update();
	virtual void Render();
	virtual void SetLightToProgram(GLProgramState& programState, int index);
};

/**
*	\brief 点光源
*/
class PointLight : public Light
{
public:
	PointLight(const Vec3i& pos, int radius, Color4B color);

	virtual void Update();
	virtual void Render();
	virtual void SetLightToProgram(GLProgramState& programState, int index);

	void Clear();
	void PushObstacle(const Rect& rect);

private:
	void CalucateLightMesh();

private:
	std::vector<Rect> mObstacles;
	std::vector<Point2> mLightMesh;	
	
	static RenderTexture mLightTexture;	/** 全局光照区域可见性贴图 */
};
