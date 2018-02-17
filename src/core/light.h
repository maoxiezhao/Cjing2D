#pragma once

#include"common\common.h"
#include"core\types.h"
#include"core\vertexArray.h"
#include"core\glProgramState.h"
#include"core\texture.h"
#include"utils\matrix4.h"
#include"utils\point.h"
#include"utils\rectangle.h"

#include<set>
#include<memory>

const int LIGHT_TYPE_BASE = 1;
const int LIGHT_TYPE_AREA = 2;
const int LIGHT_TYPE_POINT = 3;

/**
*	\brief 光照光源相关
*/
class Light
{
public:
	Light(const Vec3i& pos, int radius, Color4B color, int lightType);
	~Light();

	static void Initialize();

	virtual void Update();
	virtual void Render();
	virtual void DebugRender();
	virtual void SetLightToProgram(GLProgramState& programState, int index);

	Vec3i GetPos()const { return mPos; }
	void SetPos(const Vec3i& pos) { mPos = pos; }
	int GetRadius()const { return mRadius; }
	void SetRadius(int radius) { mRadius = radius; }
	Color4B GetColor()const { return mColor; }
	void SetColor(const Color4B& color) { mColor = color; }
	int GetType()const { return mLightType; }

protected:
	Vec3i mPos;
	int mRadius;
	Color4B mColor;
	int mLightType;
};

using LightPtr = std::shared_ptr<Light>;

/**
*	\brief 区域环境光照
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
	PointLight(const Vec3i& pos, int radius, Color4B color, float coeffi, float squaredCoeffi);

	static void Initialize();

	virtual void Update();
	virtual void Render();
	virtual void SetLightToProgram(GLProgramState& programState, int index);

	void Clear();
	void PushObstacles(const std::vector<Rect> & rects);
	
	static const int MAX_RENDER_LIGHT_COUNT = 32;
	static RenderTexture mLightTexture;	/** 全局光照区域可见性贴图 */

private:
	void RecomputeProp();
	void CalucateLightMesh();

private:
	float mCoeffi;		// 一次系数
	float mSquredCoeffi;// 二次系数

	std::vector<Rect> mObstacles;
	std::vector<Point2> mObstaclePoint;
	std::set<Point2> mObstaclePointSet;
	bool mDirty;

	Point2 mPlanePos;
	int mPlaneRadius;
	Rect mLightRect;

	static int curLightRenderIndex;
	std::vector<Point2> mLightMesh;	
};
