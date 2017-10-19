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
*	\brief ˢ������ϵͳ
*
*	��ˢ�µ�ǰ����ɾ���������ӣ�������������
*	�����Ƿ�ʹ���ڴ��ά������
*/
void ParticleSystem::Update()
{
	if (IsSuspended())
	{
		return;
	}
	// �����ǰʱ���������ϵͳ�������������
	uint32_t now = System::Now();

	// ���ȸ��µ�ǰ�������,����ʱ��ʹ��֡����ʱ�䣨ÿ֡���£�
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
		// �������Ƿ��ڣ������򲻲���������
		if (!mLooped && mDeadDate > 0 && now >= mDeadDate)
		{
			SetFinished(true);
		}
		else
		{
			// ����������
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
*	\brief �������Ӽ���
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
*	\brief �������Ӽ���
*	\param pos ���Ƶ�λ��
*/
void ParticleSystem::Draw(const Point2 & pos)
{
	// do nothing
}

/**
*	\brief ��ʼ��������
*	\param duration �����ʱ�䳤��,durationΪ0ʱ��Ϊѭ������
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
*	\brief ǿ��ֹͣ��������
*/
void ParticleSystem::Stop()
{
	SetFinished(true);
}

/**
*	\brief ��ʼ������ϵͳ
*
*	�����������������ļ�
*/
void ParticleSystem::Initialize()
{
	const string& particlePath = string("particles/") + GetParticleName() + ".dat";
	bool successed = mParticleData.ImportFromFile(particlePath);
	if (!successed)
	{
		Debug::Die("Failed to load map file '" + GetParticleName() + "'.");
	}
	// ���������ֵ
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
*	\brief �������ӽڵ�
*
*	Ŀǰparticle����ʱ���صĶ����ʵ��Ϊdrawable��Ϊ��ʵ��
*   ����entity��������Ҫʵ�����л�entity
*/
ParticleSystem::ParticleNode ParticleSystem::CreateParticleNode()
{
	ParticleNode particle;
	particle.deadDate = System::Now() + Random::GetRandomInt(
		mParticleData.GetMinLife(), mParticleData.GetMaxLife());
	
	// ��������drawable(����sprite��animaiton)��λ��״̬��
	// ���ö�Ӧ���ƶ��켣
	auto InitializeDrawable = [this](DrawablePtr& drawable, ParticleData& particleData) {
		// ���ó�ʼλ��
		Point2 posOffset = particleData.GetPosOffset();
		posOffset = Point2((int)(posOffset.x * (2 * Random::GetRandomFloat() - 1)),
					   (int)(posOffset.y * (2 * Random::GetRandomFloat() - 1)));
		Point2 realPos = posOffset + GetPos();
		drawable->SetPos(realPos);

		// �����ƶ��켣
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
		// ��ȡ���л��ļ�����entity����δʵ��
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
*	\brief ��������ʱ��Ԥ�������
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
