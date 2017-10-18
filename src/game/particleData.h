#pragma once

#include"lua\luaData.h"
#include"game\enumInfo.h"
#include"utils\point.h"

/**
*	\brief 粒子属性数据
*/
class ParticleData : public LuaData
{
public:
	ParticleData();

	/**
	*	\brief 搭载的实体对象类型
	*/
	enum ParticleType
	{
		SPRITE,
		ANIMATION,
		ENTITY
	};

	/**** ****  setter/getter **** ****/
	void SetMinSpeed(uint32_t minSpeed);
	uint32_t GetMinSpeed()const;
	void SetMaxSpeed(uint32_t maxSpeed);
	uint32_t GetMaxSpeed()const;
	void SetMinGravity(double minGravity);
	double GetMinGravity()const;
	void SetMaxGravity(double maxGravity);
	double GetMaxGravity()const;
	void SetMinLife(uint32_t minLife);
	uint32_t GetMinLife()const;
	void SetMaxLife(uint32_t maxLife);
	uint32_t GetMaxLife()const;
	ParticleType GetType()const;
	void SetType(ParticleType type);
	const string& GetEntityPath()const;
	void SetEntityPath(const string& entityPath);
	void SetEmissionRate(double emissionRate);
	double GetEmissionRate()const;
	void SetPosOffset(const Point2& posOffset);
	Point2 GetPosOffset()const;
	void SetStartSpin(double startSpin);
	double GetStartSpin()const;
	void SetEndSpin(double endSpin);
	double GetEndSpin()const;
	void SetMinRadialAccel(double minRadialAccel);
	double GetMinRadialAccel()const;
	void SetMaxRadialAccel(double maxRadialAccel);
	double GetMaxRadialAccel()const;
	void SetMinTangentialAccel(double minTangentialAccel);
	double GetMinTangentialAccel()const;
	void SetMaxTangentialAccel(double maxTangentialAccel);
	double GetMaxTangentialAccel()const;

	virtual bool ImportFromLua(lua_State*l);
private:
	static int LuaParticle(lua_State* l);

	/**** particle perperty ****/
	ParticleType mType;
	string mEntityPath;			/** 当前粒子搭载的实体的所指向的路径 具体数据由type决定，
									当type为drawable，该路径为图片路径。当type为entity时，
									该路劲为序列化数据文件*/
	double mEmissionRate;		/** 每秒产生的粒子速率 */

	uint32_t mMinSpeed;
	uint32_t mMaxSpeed;

	uint32_t mMinLife;
	uint32_t mMaxLife;

	double mMinGravity;
	double mMaxGravity;

	double mStartSpin;
	double mEndSpin;

	int mStartAplha;
	int mEndAlpha;

	Point2 mPosOffset;		    /** 生成粒子位置的偏差值 */

	double mMinRadialAccel;	
	double mMaxRadialAccel;

	double mMinTangentialAccel;	
	double mMaxTangentialAccel;
};

template<>
struct EnumInfoTraits<ParticleData::ParticleType>
{
	static const string prettyName;
	static const EnumInfo<ParticleData::ParticleType>::nameType names;
};