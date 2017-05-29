#include "animationData.h"
#include "core\debug.h"
#include "lua\luaTools.h"

AnimationData::AnimationData()
{
}

AnimationData::AnimationData(const string & imageName, const std::deque<AnimationDirectionData>& directions, uint32_t frameDelay, int frameLoop):
	mImageName(imageName),
	mDirections(directions),
	mFrameDelay(frameDelay),
	mFrameLoop(frameLoop)
{
}

string AnimationData::GetImageName() const
{
	return mImageName;
}

uint32_t AnimationData::GetFrameDelay() const
{
	return mFrameDelay;
}

int AnimationData::GetFrameLoop() const
{
	return mFrameLoop;
}

const std::deque<AnimationDirectionData>& AnimationData::GetDirections()const 
{
	return mDirections;
}

AnimationLuaData::AnimationLuaData()
{
}

/**
*	\brief 从Lua文件中导入数据
*
*	将当前AnimationLuaData保存在LUA_REGISTRE,同时根据格式将
*	animation注册上解析函数，在解析函数存储数据
*/
bool AnimationLuaData::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);	// lightuserdata
	lua_setfield(l, LUA_REGISTRYINDEX, "Animation");
	lua_register(l, "animation", LuaAnimation);
	if (lua_pcall(l,0,0,0) != 0)
	{
		Debug::Error("Failed to load animation datafile.");
		lua_pop(l,1);	// ?????
		return false;
	}
	return true;
}

/**
*	\brief animation格式解析
*/
int AnimationLuaData::LuaAnimation(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "Animation");
		AnimationLuaData* animationData = static_cast<AnimationLuaData*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		LuaTools::CheckType(l, 1, LUA_TTABLE);
		string animationName = LuaTools::CheckFieldString(l, 1, "name");
		string srcImage = LuaTools::CheckFieldString(l, 1, "src_image");
		uint32_t frameDelay = LuaTools::CheckFieldIntByDefault(l, 1, "frame_delay", 0);
		int frameLoop = LuaTools::CheckFieldInt(l, 1, "frame_loop");

		if (frameLoop < -1)
		{
			LuaTools::ArgError(l, 1,
				"Bad field 'frameloop',frame must be positive or -1.");
		}

		lua_settop(l, 1);
		lua_getfield(l, 1, "directions");
		if (lua_type(l, 2) != LUA_TTABLE)
		{
			LuaTools::ArgError(l, 1,
				string("Bad filed 'directions',(excepted table,got ") + lua_typename(l, -1) + ")");
		}

		// 方向数据解析
		std::deque<AnimationDirectionData> directions;
		int directionIndex = 1;
		lua_rawgeti(l, -1, directionIndex);
		while (!lua_isnil(l, -1))
		{
			++directionIndex;

			if (lua_type(l, -1) != LUA_TTABLE)
			{
				LuaTools::ArgError(l, 1,
					string("Bad filed 'directions',(excepted table,got ") + lua_typename(l, -1) + ")");
			}

			// 单方向数据解析
			int x = LuaTools::CheckFieldInt(l, -1, "x");
			int y = LuaTools::CheckFieldInt(l, -1, "y");
			int width = LuaTools::CheckFieldInt(l, -1, "width");
			int height = LuaTools::CheckFieldInt(l, -1, "height");
			int orginX = LuaTools::CheckFieldIntByDefault(l, -1, "orgin_x", 0);
			int orginY = LuaTools::CheckFieldIntByDefault(l, -1, "orgin_y", 0);
			int numFrames = LuaTools::CheckFieldIntByDefault(l, -1, "num_frames", 0);
			int numColumns = LuaTools::CheckFieldInt(l, -1, "num_columns");

			if (numColumns < 1 || numColumns > numFrames)
			{
				LuaTools::ArgError(l, 1,
					string("Bad field 'num_colums',must be between 1 and num_frames."));
			}
			lua_pop(l, 1);
			lua_rawgeti(l, -1, directionIndex);

			directions.emplace_back(Point2(x, y), Point2(orginX, orginY),
				Size(width, height), numFrames, numColumns);

		}
	
		if (animationData->mAnimations.find(animationName) != animationData->mAnimations.end())
		{
			LuaTools::Error(l, string("Duplicate animation name '") + animationName + "'");
		}
		animationData->mAnimations.emplace(animationName, 
			AnimationData(srcImage, directions, frameDelay, frameLoop));
		
		// 如果载入的是一个动画，则设置默认动画名
		if (animationData->mAnimations.size() == 1)
		{
			animationData->mDefaultName = animationName;
		}
	});

}

const std::map<string, AnimationData>& AnimationLuaData::GetAnimations() const
{
	return mAnimations;
}

string AnimationLuaData::GetDefaultName() const
{
	return mDefaultName;
}

