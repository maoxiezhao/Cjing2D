#include "game\particleData.h"
#include "core\debug.h"

const string EnumInfoTraits<ParticleData::ParticleType>::prettyName = "ParticleType";
const EnumInfo<ParticleData::ParticleType>::nameType EnumInfoTraits<ParticleData::ParticleType>::names = {
	{ParticleData::ENTITY, "entity"},
	{ParticleData::ANIMATION, "animation"},
	{ParticleData::SPRITE, "sprite"}
};

ParticleData::ParticleData():
	mMinSpeed(0),
	mMaxSpeed(0),
	mMinLife(0),
	mMaxLife(0),
	mMinGravity(0.0),
	mMaxGravity(0.0),
	mStartAplha(0),
	mEndAlpha(0),
	mMinRadialAccel(0.0),
	mMaxRadialAccel(0.0),
	mMinTangentialAccel(0.0),
	mMaxTangentialAccel(0.0)
{
}

/**
*	\brief 从.dat Particle文件中加载数据
*/
bool ParticleData::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "particleData");
	lua_register(l, "particle", LuaParticle);
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Failed to load particle data.");
		lua_pop(l, 1);
		return false;
	}
	return true;
}

/**
*	\brief 解析粒子property
*/
int ParticleData::LuaParticle(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "particleData");
		ParticleData* particleData = static_cast<ParticleData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		const std::string& name = LuaTools::CheckFieldString(l, 1, "name");
		int minLfie = LuaTools::CheckFieldInt(l, 1, "minLife");
		int maxLife = LuaTools::CheckFieldInt(l, 1, "maxLife");
		double minGravity = LuaTools::CheckFieldNumber(l, 1, "minGravity");
		double maxGravity = LuaTools::CheckFieldNumber(l, 1, "maxGravity");
		int minSpeed = LuaTools::CheckFieldInt(l, 1, "minSpeed");
		int maxSpeed = LuaTools::CheckFieldInt(l, 1, "maxSpeed");
		double emissionRate = LuaTools::CheckFieldNumber(l, 1, "emissionRate");
		ParticleType type = LuaTools::CheckFiledEnum<ParticleType>(l, 1, "type");
		const std::string& path = LuaTools::CheckFieldString(l, 1, "path");
		Point2 posOffset = LuaTools::CheckFieldPoint2(l, 1, "posOffset");
		double startSpin = LuaTools::CheckFieldNumber(l, 1, "startSpin");
		double endSpin = LuaTools::CheckFieldNumber(l, 1, "endSpin");
		double minRadial = LuaTools::CheckFieldNumber(l, 1, "minRadialAccel");
		double maxRadial = LuaTools::CheckFieldNumber(l, 1, "maxRadialAccel");
		double minTangential = LuaTools::CheckFieldNumber(l, 1, "minTangentialAccel");
		double maxTangential = LuaTools::CheckFieldNumber(l, 1, "maxTangentialAccel");

		particleData->SetType(type);
		particleData->SetEntityPath(path);
		particleData->SetMinLife(minLfie);
		particleData->SetMaxLife(maxLife);
		particleData->SetMinGravity(minGravity);
		particleData->SetMaxGravity(maxGravity);
		particleData->SetMinSpeed(minSpeed);
		particleData->SetMaxSpeed(maxSpeed);
		particleData->SetEmissionRate(emissionRate);
		particleData->SetPosOffset(posOffset);
		particleData->SetStartSpin(startSpin);
		particleData->SetEndSpin(endSpin);
		particleData->SetMinRadialAccel(minRadial);
		particleData->SetMaxRadialAccel(maxRadial);
		particleData->SetMinTangentialAccel(minTangential);
		particleData->SetMaxTangentialAccel(maxTangential);

		return 0;
	});
}

void ParticleData::SetMinSpeed(uint32_t minSpeed)
{
	mMinSpeed = minSpeed;
}

uint32_t ParticleData::GetMinSpeed() const
{
	return mMinSpeed;
}

void ParticleData::SetMaxSpeed(uint32_t maxSpeed)
{
	mMaxSpeed = maxSpeed;
}

uint32_t ParticleData::GetMaxSpeed() const
{
	return mMaxSpeed;
}

void ParticleData::SetMinGravity(double minGravity)
{
	mMinGravity = minGravity;
}

double ParticleData::GetMinGravity() const
{
	return mMinGravity;
}

void ParticleData::SetMaxGravity(double maxGravity)
{
	mMaxGravity = maxGravity;
}

double ParticleData::GetMaxGravity() const
{
	return mMaxGravity;
}

void ParticleData::SetMinLife(uint32_t minLife)
{
	mMinLife = minLife;
}

uint32_t ParticleData::GetMinLife() const
{
	return mMinLife;
}

void ParticleData::SetMaxLife(uint32_t maxLife)
{
	mMaxLife = maxLife;
}

uint32_t ParticleData::GetMaxLife() const
{
	return mMaxLife;
}

ParticleData::ParticleType ParticleData::GetType() const
{
	return mType;
}

void ParticleData::SetType(ParticleType type)
{
	mType = type;
}

const string & ParticleData::GetEntityPath() const
{
	return mEntityPath;
}

void ParticleData::SetEntityPath(const string & entityPath)
{
	mEntityPath = entityPath;
}

void ParticleData::SetEmissionRate(double emissionRate)
{
	mEmissionRate = emissionRate;
}

double ParticleData::GetEmissionRate() const
{
	return mEmissionRate;
}

void ParticleData::SetPosOffset(const Point2& posOffset)
{
	mPosOffset = posOffset;
}

Point2 ParticleData::GetPosOffset() const
{
	return mPosOffset;
}

void ParticleData::SetStartSpin(double startSpin)
{
	mStartSpin = startSpin;
}

double ParticleData::GetStartSpin() const
{
	return mStartSpin;
}

void ParticleData::SetEndSpin(double endSpin)
{
	mEndSpin = endSpin;
}

double ParticleData::GetEndSpin() const
{
	return mEndSpin;
}

void ParticleData::SetMinRadialAccel(double minRadialAccel)
{
	mMinRadialAccel = minRadialAccel;
}

double ParticleData::GetMinRadialAccel() const
{
	return mMinRadialAccel;
}

void ParticleData::SetMaxRadialAccel(double maxRadialAccel)
{
	mMaxRadialAccel = maxRadialAccel;
}

double ParticleData::GetMaxRadialAccel() const
{
	return mMaxRadialAccel;
}

void ParticleData::SetMinTangentialAccel(double minTangentialAccel)
{
	mMinTangentialAccel = minTangentialAccel;
}

double ParticleData::GetMinTangentialAccel() const
{
	return mMinTangentialAccel;
}

void ParticleData::SetMaxTangentialAccel(double maxTangentialAccel)
{
	mMaxTangentialAccel = maxTangentialAccel;
}

double ParticleData::GetMaxTangentialAccel() const
{
	return mMaxTangentialAccel;
}
