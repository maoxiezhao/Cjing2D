#include "light.h"
#include "core\renderer.h"
#include "core\video.h"
#include "utils\matrix4.h"

Light::Light(const Vec3i & pos, int radius, Color4B color, int lightType):
	mPos(pos),
	mRadius(radius),
	mColor(color),
	mLightType(lightType)
{
}

Light::~Light()
{
}

/**
*	\brief light初始化
*/
void Light::Initialize()
{
	PointLight::Initialize();
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
	Light(pos, radius, color, LIGHT_TYPE_AREA)
{
}

void AreaLight::Update()
{
	// nothing
}

void AreaLight::Render()
{
	// nothing
}

void AreaLight::SetLightToProgram(GLProgramState & programState, int index)
{
	programState.SetUniform3f("ambientColor", GetColor().GetColorRGB3F());
}

int PointLight::curLightRenderIndex = 0;
RenderTexture PointLight::mLightTexture;	/** 全局光照区域可见性贴图 */

PointLight::PointLight(const Vec3i & pos, int radius, Color4B color, float coeffi, float squaredCoeffi):
	Light(pos, radius, color, LIGHT_TYPE_POINT),
	mDirty(false),
	mCoeffi(coeffi),
	mSquredCoeffi(squaredCoeffi)
{
	RecomputeProp();
}

/**
*	\brief 点光源初始化
*/
void PointLight::Initialize()
{
	const Size size = Video::GetScreenSize();
	mLightTexture.SetInternalFormat(GL_RGBA);
	mLightTexture.SetImageFormat(GL_RGBA);
	mLightTexture.SetIntenalType(GL_UNSIGNED_BYTE);
	mLightTexture.InitWithSize(size.width, size.height, false);
}

/**
*	\brief 更新点光源
*
*	目前设定光源的更新处于所有的游戏逻辑之后,在entities中调用
*/
void PointLight::Update()
{
	if (mDirty)
	{
		CalucateLightMesh();
	}
}


/**
*	\brief 绘制点光源
*
*	目前设定光源的绘制处于所有的游戏逻辑绘制之后点光源的绘制，
*	主要是绘制对应的光照区域图，在renderer中调用
*/
void PointLight::Render()
{
	PolygonVertex pv;
	Color4B color = Color4B::WHITE;
	for (auto& point : mLightMesh)
	{
		V3F_C4B v(point.x, point.y, 0.0f, color.r, color.g, color.b, color.a);
		pv.push_back(v);
	}

	Renderer::GetInstance().PushPolygon(pv);
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
	programState.SetUniform1f(lightStr + "Coeffi", mCoeffi);
	programState.SetUniform1f(lightStr + "SquaredCoeffi", mSquredCoeffi);
}

void PointLight::Clear()
{
	mObstacles.clear();
	mObstaclePoint.clear();
	mLightMesh.clear();
}

/**
*	\brief 添加光线遮挡体，同时进行dirty检测，避免重复生成
*/
void PointLight::PushObstacles(const std::vector<Rect>& rects)
{
	std::set<Point2> sets = mObstaclePointSet;
	mObstaclePointSet.clear();
	mObstaclePoint.clear();

	for (auto& rect : rects)
	{
		for (auto& point : rect.GetPoints())
		{
			if (!mDirty && sets.find(point) != sets.end())
				mDirty = true;

			mObstaclePoint.push_back(point);
			mObstaclePointSet.insert(point);
		}
		mObstacles.push_back(rect);
	}

	if (!mDirty && sets.size() != mObstaclePointSet.size())
		mDirty = true;
}

/**
*	\brief 重新计算属性
*/
void PointLight::RecomputeProp()
{
	mPlanePos = Point2(mPos.x, mPos.y);
	mPlaneRadius = (int)sqrt(mRadius * mRadius - mPos.z * mPos.z);

	int width = mPlaneRadius * 2;
	mLightRect.SetSize(width, width);
	mLightRect.SetCenter(mPos.x, mPos.y);

	mDirty = true;
}

/**
*	\brief 计算光照区域网格
*
*	在渲染阶段调用该函数计算光照可见性区域网格，对
*	区域内每个rect顶点进行射线检测
*/
void PointLight::CalucateLightMesh()
{
	if (!mDirty)
		return;

	mLightMesh.clear();

	// 0.默认添加light范围rect
	for (auto& point : mLightRect.GetPoints())
	{
		mObstaclePoint.push_back(point);
	}
	mObstacles.push_back(mLightRect);

	Point2 origin = mPlanePos;
	const float DiffDegree = 0.5f;
	auto GenerateRay = [&origin, &DiffDegree](std::vector<Ray>& rays, const Point2& point){
		// 需要生成对应顶点一条射线和上下偏移一度的射线
		Ray ray(origin, point - origin);
		rays.push_back(ray);
		Matrix4 rotateMat = Matrix4::RotateZ(DiffDegree);
		Ray diffRay = ray;
		rotateMat.Transform(ray, diffRay);
		rays.push_back(diffRay);
		rotateMat = Matrix4::RotateZ(-DiffDegree);
		rotateMat.Transform(ray, diffRay);
		rays.push_back(diffRay);
	};

	// 1.根据rect生成射线
	std::vector<Ray> rays;
	for (auto& point : mObstaclePoint)
	{
		GenerateRay(rays, point);
	}

	// 2.排序ray,根据角度排序
	std::sort(rays.begin(), rays.end(), [](Ray& ray1, Ray& ray2) {
		return ray1.mDegree < ray2.mDegree;
	});

	// 3.遍历rect，计算射线交点
	for (auto& ray : rays)
	{
		bool bIntersect = false;
		float mint = FLT_MAX;
		float tmpMint = 0.f, tmpMaxt = 0.f;
		for (auto& rect : mObstacles)
		{
			bIntersect |= rect.Intersect(ray, tmpMint, tmpMaxt);
			if (bIntersect)
				mint = std::min(mint, tmpMint);
		}
		if (bIntersect)
		{
			Point2 intersectPoint = ray.GetPosition(mint);
			mLightMesh.push_back(intersectPoint);
		}
	}

	mObstacles.clear();
	mDirty = false;
}
