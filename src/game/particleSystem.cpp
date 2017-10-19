#include"game\particleSystem.h"
#include"core\system.h"
#include"core\debug.h"
#include"core\random.h"
#include"movements\accelMovement.h"
#include"utils\geometry.h"
#include"lua\luaContext.h"

ParticleSystem::ParticleSystem():
	mParticleFileName(""),
	mLifeTime(0),
	mDeadDate(0),
	mEmissionRate(0.0),
	mEmissionCountInPerFrame(0),
	mLastEmissionWaste(0),
	mIsFinished(true),
	mIsInitialized(false),
	mLooped(false)
{
}

ParticleSystem::ParticleSystem(const std::string & ParticleSystemFile):
	mParticleFileName(ParticleSystemFile),
	mLifeTime(0),
	mDeadDate(0),
	mEmissionRate(0.0),
	mEmissionCountInPerFrame(0),
	mLastEmissionWaste(0),
	mIsFinished(true),
	mIsInitialized(false),
	mLooped(false)
{
}

ParticleSystem::~ParticleSystem()
{
	if (mIsInitialized)
	{
		Quit();

		for (auto& itor = mParticles.begin(); itor != mParticles.end();)
		{
			itor = mParticles.erase(itor);
		}
		mParticles.clear();
	}
}

/**
*	\brief 刷新粒子系统
*
*	会刷新当前粒子删除过期粒子，并产生新粒子
*	考虑是否使用内存池维护粒子
*/
void ParticleSystem::Update()
{
	if (IsSuspended())
	{
		return;
	}
	// 如果当前时间大于粒子系统声明周期则结束
	uint32_t now = System::Now();

	// 首先更新当前存活粒子,这里时间使用帧更新时间（每帧更新）
	for (auto& itor = mParticles.begin(); itor != mParticles.end();)
	{
		auto& particle = *itor;
		if (now >= particle.deadDate)
		{
			itor = mParticles.erase(itor);
			continue;
		}
		if (particle.drawable != nullptr)
		{
			particle.drawable->Update();
		}
		else if (particle.entity != nullptr)
		{
			particle.entity->Update();
		}
		itor++;
	}

	if (!IsFinished())
	{
		// 发生器是否到期，到期则不产生新粒子
		if (!mLooped && mDeadDate > 0 && now >= mDeadDate)
		{
			SetFinished(true);
		}
		else
		{
			// 生成新粒子
			int emissionCount = (int)(mEmissionCountInPerFrame*System::timeStep + mLastEmissionWaste);
			mLastEmissionWaste = (mEmissionCountInPerFrame*System::timeStep + mLastEmissionWaste) - (double)(emissionCount);
			for (int particleIndex = 0; particleIndex < emissionCount; particleIndex++)
			{
				if (mParticles.size() >= mMaxParticleNum)
				{
					break;
				}
				ParticleNode particle = CreateParticleNode();
				mParticles.push_back(particle);
			}
		}
	}
}

/**
*	\brief 绘制粒子集合
*/
void ParticleSystem::Draw()
{
	for (auto& particle : mParticles)
	{
		if (particle.entity != nullptr)
		{
			particle.entity->Draw();
		}
		else if (particle.drawable != nullptr)
		{
			particle.drawable->Draw();
		}
	}
}

/**
*	\brief 绘制粒子集合
*	\param pos 绘制的位置
*/
void ParticleSystem::Draw(const Point2 & pos)
{
	// do nothing
}

/**
*	\brief 开始发射粒子
*	\param duration 发射的时间长度,duration为0时，为循环播放
*/
void ParticleSystem::Start(uint32_t duration)
{
	Debug::CheckAssertion(mIsInitialized,
		"Start particle without initialized.");

	uint32_t now = System::Now();
	mDeadDate = now + duration;
	mLooped = duration > 0 ? false : true;

	SetFinished(false);
}

/**
*	\brief 强制停止产生粒子
*/
void ParticleSystem::Stop()
{
	SetFinished(true);
}

/**
*	\brief 初始化粒子系统
*
*	加载粒子描述数据文件
*/
void ParticleSystem::Initialize()
{
	const string& particlePath = string("particles/") + GetParticleName() + ".dat";
	bool successed = mParticleData.ImportFromFile(particlePath);
	if (!successed)
	{
		Debug::Die("Failed to load map file '" + GetParticleName() + "'.");
	}
	// 计算相关数值
	mEmissionRate = mParticleData.GetEmissionRate();
	if (mEmissionRate<= 0)
	{
		Debug::Error("Invalid particle emission rate.");
		return;
	}
	mEmissionCountInPerFrame = mEmissionRate / 1000;
	mIsInitialized = true;
}

void ParticleSystem::Quit()
{
}

/**
*	\brief 创建粒子节点
*
*	目前particle创建时搭载的对象仅实现为drawable，为了实现
*   搭载entity，可能需要实现序列化entity
*/
ParticleSystem::ParticleNode ParticleSystem::CreateParticleNode()
{
	ParticleNode particle;
	particle.deadDate = System::Now() + Random::GetRandomInt(
		mParticleData.GetMinLife(), mParticleData.GetMaxLife());
	
	// 用于设置drawable(包括sprite和animaiton)的位置状态和
	// 设置对应的移动轨迹
	auto InitializeDrawable = [this](DrawablePtr& drawable, ParticleData& particleData) {
		// 设置初始位置
		Point2 posOffset = particleData.GetPosOffset();
		posOffset = Point2((int)(posOffset.x * (2 * Random::GetRandomFloat() - 1)),
					   (int)(posOffset.y * (2 * Random::GetRandomFloat() - 1)));
		Point2 realPos = posOffset + GetPos();
		drawable->SetPos(realPos);

		// 设置移动轨迹
		double spin = Random::GetRandomDouble(particleData.GetStartSpin(), particleData.GetEndSpin());
		double speed = Random::GetRandomInt(particleData.GetMinSpeed(), particleData.GetMaxSpeed());
		double radialAccel = Random::GetRandomDouble(particleData.GetMinRadialAccel(), particleData.GetMaxRadialAccel());
		double tangentialAccel = Random::GetRandomDouble(particleData.GetMinTangentialAccel(), particleData.GetMaxTangentialAccel());
		double gravityAccel = Random::GetRandomDouble(particleData.GetMinGravity(), particleData.GetMaxGravity());		

		std::shared_ptr<AccelMovement> movement =
			std::make_shared<AccelMovement>(radialAccel, tangentialAccel, gravityAccel, true);
		movement->SetOrigin(realPos);
		movement->SetAngle(Geometry::Radians(spin));
		movement->SetSpeed(speed);

		drawable->StartMovement(movement);
	};

	auto type = mParticleData.GetType();
	if (type == ParticleData::ENTITY)
	{
		// 读取序列化文件创建entity，暂未实现
	}
	else if (type == ParticleData::SPRITE)
	{
		const string& spritePath = mParticleData.GetEntityPath();
		SpritePtr sprite = std::make_shared<Sprite>(spritePath);
		particle.drawable = sprite;
		InitializeDrawable(particle.drawable, mParticleData);
	}
	else if (type == ParticleData::ANIMATION)
	{
		const string& animationPath = mParticleData.GetEntityPath();
		AnimationSpritePtr animation = std::make_shared<AnimationSprite>(animationPath);
		animation->StartAnimation();
		particle.drawable = animation;
		InitializeDrawable(particle.drawable, mParticleData);
	}
	else
	{
		Debug::Error("Invalid particle type.");
	}
	Preprocess(particle);
	return particle;
}

/**
*	\brief 产生粒子时的预处理操作
*/
void ParticleSystem::Preprocess(ParticleNode& particle)
{
	if (!mPreprocessFunc.IsEmpty())
	{
		LuaContext* luaContext = GetLuaContext();
		if (luaContext != nullptr)
		{
			lua_State*l = mPreprocessFunc.GetLuaState();
			mPreprocessFunc.Push();
			luaContext->PushDrawable(l, *this);
			LuaTools::CallFunction(l, 1, 0, "Praticle preprocess.");
		}
	}
}

const string ParticleSystem::GetLuaObjectName() const
{
	return LuaContext::module_particle_name;
}
