#pragma once

#include"common\common.h"
#include"lua\luaObject.h"
#include"game\drawable.h"
#include"game\particleData.h"
#include"entity\entity.h"

/**
*	\brief ����ϵͳ
*	����ϵͳ��Ϊ���ӷ��������������������ӣ�ͬʱ����
*   ��drawalbe���������movement������lua�ű���
*
*	2017.10.17
*	Ŀǰ���ӷ�����֧����drawable��entity��Ϊ���ӹ���
*	���еĻ����ɸ���ʵ������ʵ�֣����ܴ����������⣬
*   δ����ʵ��һ�����õ��������Ʒ�ʽ
*/
class ParticleSystem : public Drawable
{
public:
	ParticleSystem();
	ParticleSystem(const std::string& particleFile);
	~ParticleSystem();

	virtual void Update();
	virtual void Draw();
	virtual void Draw(const Point2& pos);
	
	void Start(uint32_t duration);
	void Initialize();
	void Quit();

	// lua
	virtual const string GetLuaObjectName()const;

	/**** **** status manager **** ****/
	bool IsFinished()const;
	void SetFinished(bool finished);
	const string& GetParticleName()const;

private:
	/**
	*	\brief ���ӽڵ�,ParticleSystem�����ӽڵ�
	*	��Ϊ����Ļ�����λ������ǰʱ�䵽�������
	*	����������ʱ��������
	*/
	struct ParticleNode
	{
		DrawablePtr drawable;
		EntityPtr entity;
		uint32_t deadDate;		/** ���ӵ�����ʱ�� */

		ParticleNode() :drawable(nullptr), entity(nullptr), deadDate(0) {}
	};

	std::list<ParticleNode> mParticles;				/** ��ǰ���д������ */

private:
	ParticleNode CreateParticleNode();
	void Preprocess(ParticleNode& particle);// ��ʵ���Զ���Ĵ�������������¶��luaִ��
	LuaRef mPreprocessFunc;			/** ��������ʱԤ�����lua���� */

	string mParticleFileName;
	uint32_t mLifeTime;
	uint32_t mDeadDate;
	uint32_t mMaxParticleNum;

	double mEmissionRate;     	    /** �������ʣ�������� */
	double mEmissionCountInPerFrame;/** ÿ֡���ɵĸ��� */
	double mLastEmissionWaste;		/** ÿ�����ɺ�ʣ��ķ��ϸ��� */

	bool mIsInitialized;
	bool mIsFinished;				/** �Ƿ��Ѿ��������ӷ������ */
	ParticleData mParticleData;		/** �������ݣ�Ӧ��initialize�д��� */
};

using ParticleSystemPtr = std::shared_ptr<ParticleSystem>;

inline bool ParticleSystem::IsFinished()const
{
	return mIsFinished;
}

inline void ParticleSystem::SetFinished(bool finished)
{
	mIsFinished = finished;
}

inline const string & ParticleSystem::GetParticleName() const
{
	return mParticleFileName;
}
