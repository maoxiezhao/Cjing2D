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
*	\brief light��ʼ��
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
*	\brief ����Դ���ݴ��͵�program
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
RenderTexture PointLight::mLightTexture;	/** ȫ�ֹ�������ɼ�����ͼ */

PointLight::PointLight(const Vec3i & pos, int radius, Color4B color, float coeffi, float squaredCoeffi):
	Light(pos, radius, color, LIGHT_TYPE_POINT),
	mDirty(false),
	mCoeffi(coeffi),
	mSquredCoeffi(squaredCoeffi)
{
	RecomputeProp();
}

/**
*	\brief ���Դ��ʼ��
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
*	\brief ���µ��Դ
*
*	Ŀǰ�趨��Դ�ĸ��´������е���Ϸ�߼�֮��,��entities�е���
*/
void PointLight::Update()
{
	if (mDirty)
	{
		CalucateLightMesh();
	}
}


/**
*	\brief ���Ƶ��Դ
*
*	Ŀǰ�趨��Դ�Ļ��ƴ������е���Ϸ�߼�����֮����Դ�Ļ��ƣ�
*	��Ҫ�ǻ��ƶ�Ӧ�Ĺ�������ͼ����renderer�е���
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
*	\brief ��ӹ����ڵ��壬ͬʱ����dirty��⣬�����ظ�����
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
*	\brief ���¼�������
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
*	\brief ���������������
*
*	����Ⱦ�׶ε��øú���������տɼ����������񣬶�
*	������ÿ��rect����������߼��
*/
void PointLight::CalucateLightMesh()
{
	if (!mDirty)
		return;

	mLightMesh.clear();

	// 0.Ĭ�����light��Χrect
	for (auto& point : mLightRect.GetPoints())
	{
		mObstaclePoint.push_back(point);
	}
	mObstacles.push_back(mLightRect);

	Point2 origin = mPlanePos;
	const float DiffDegree = 0.5f;
	auto GenerateRay = [&origin, &DiffDegree](std::vector<Ray>& rays, const Point2& point){
		// ��Ҫ���ɶ�Ӧ����һ�����ߺ�����ƫ��һ�ȵ�����
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

	// 1.����rect��������
	std::vector<Ray> rays;
	for (auto& point : mObstaclePoint)
	{
		GenerateRay(rays, point);
	}

	// 2.����ray,���ݽǶ�����
	std::sort(rays.begin(), rays.end(), [](Ray& ray1, Ray& ray2) {
		return ray1.mDegree < ray2.mDegree;
	});

	// 3.����rect���������߽���
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
