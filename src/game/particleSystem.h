#pragma once

#include"common\common.h"
#include"lua\luaObject.h"
#include"game\drawable.h"
#include"game\particleData.h"
#include"entity\entity.h"

/**
*	\brief 粒子系统
*	粒子系统作为粒子发生器，管理生成销粒子，同时派生
*   自drawalbe，可以添加movement，绑定至lua脚本中
*
*	2017.10.17
*	目前粒子发生器支持以drawable和entity作为粒子管理，
*	所有的绘制由各个实体自身实现，可能存在性能问题，
*   未来会实现一个更好的批量绘制方式
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
	*	\brief 粒子节点,ParticleSystem以粒子节点
	*	作为管理的基本单位，当当前时间到达该粒子
	*	的声明周期时，则销毁
	*/
	struct ParticleNode
	{
		DrawablePtr drawable;
		EntityPtr entity;
		uint32_t deadDate;		/** 粒子的死亡时间 */

		ParticleNode() :drawable(nullptr), entity(nullptr), deadDate(0) {}
	};

	std::list<ParticleNode> mParticles;				/** 当前所有存活粒子 */

private:
	ParticleNode CreateParticleNode();
	void Preprocess(ParticleNode& particle);// 将实现自定义的创建函数，并暴露给lua执行
	LuaRef mPreprocessFunc;			/** 产生粒子时预处理的lua函数 */

	string mParticleFileName;
	uint32_t mLifeTime;
	uint32_t mDeadDate;
	uint32_t mMaxParticleNum;

	double mEmissionRate;     	    /** 产生速率，按秒计算 */
	double mEmissionCountInPerFrame;/** 每帧生成的个数 */
	double mLastEmissionWaste;		/** 每次生成后剩余的废料个数 */

	bool mIsInitialized;
	bool mIsFinished;				/** 是否已经结束粒子发射过程 */
	ParticleData mParticleData;		/** 粒子数据，应在initialize中创建 */
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
