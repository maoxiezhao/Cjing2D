#include "animationData.h"

AnimationData::AnimationData()
{
}

AnimationLuaData::AnimationLuaData()
{
}

bool AnimationLuaData::ImportFromLua(lua_State * l)
{
	return true;
}

const std::map<string, AnimationData>& AnimationLuaData::GetAnimations() const
{
	return mAnimations;
}

