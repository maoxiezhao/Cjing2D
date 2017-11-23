#include"luaContext.h"
#include"game\animationSprite.h"

/**
*	\brief animation lua�ӿ�,������sprite
*/
const string LuaContext::module_animation_name = "Animation";

void LuaContext::RegisterAnimationModule()
{
	static const luaL_Reg function[] = {
		{"create", animation_api_create},
		{nullptr,nullptr}
	};

	static const luaL_Reg methods[] = {
		{"setAnimation", animation_api_set_animation},
		{"setDirection", animation_api_set_direction},
		{"setSize", animation_api_set_size},
		{"start", animation_api_play_animation},
		{"stop", animation_api_stop_animation},
		{"draw", animation_api_draw },
		{nullptr,nullptr}
	};

	static const luaL_Reg metamethods[] = {
		{ "__gc", drawable_meta_api_gc },
		{ nullptr, nullptr }
	};

	RegisterType(l, module_animation_name,module_sprite_name,
		function,methods, metamethods);
}

void LuaContext::PushAnimation(lua_State*l, AnimationSprite& animation)
{
	PushUserdata(l, animation);
}

AnimationSpritePtr LuaContext::CheckAnimation(lua_State*l, int index)
{
	return std::static_pointer_cast<AnimationSprite>(CheckUserdata(l, index, module_animation_name));
}

bool LuaContext::IsAnimation(lua_State * l, int index)
{
	return IsUserdata(l, index, module_animation_name);
}

/**
*	\brief ʵ��cjing.Animation.create(animationID)
*/
int LuaContext::animation_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string& animationID = LuaTools::CheckString(l, 1);

		AnimationSpritePtr animation = std::make_shared<AnimationSprite>(animationID);
		GetLuaContext(l).AddDrawable(animation);

		PushAnimation(l, *animation);
		return 1;
	});
}

/**
*	\brief ʵ��cjing.Animation.setAnimation(animationName)
*/
int LuaContext::animation_api_set_animation(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string& animationName = LuaTools::CheckString(l, 1);

		AnimationSprite& animation = *CheckAnimation(l, 1);
		animation.SetCurrAnimation(animationName);

		return 0;
	});
}

int LuaContext::animation_api_set_direction(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		int direction = LuaTools::CheckInt(l, 1);

		AnimationSprite& animation = *CheckAnimation(l, 1);
		animation.SetCurrDirection(direction);

		return 0;
	});
}

int LuaContext::animation_api_set_size(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AnimationSprite& animation = *CheckAnimation(l, 1);
		int x = LuaTools::CheckInt(l, 2);
		int y = LuaTools::CheckInt(l, 3);
		animation.SetSize({ x, y });

		return 0;
	});
}

int LuaContext::animation_api_play_animation(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AnimationSprite& animation = *CheckAnimation(l, 1);
		animation.StartAnimation();

		return 0;
	});
}

int LuaContext::animation_api_stop_animation(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AnimationSprite& animation = *CheckAnimation(l, 1);
		animation.StopAnimation();

		return 0;
	});
}

int LuaContext::animation_api_draw(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AnimationSprite& animation = *CheckAnimation(l, 1);
		animation.Draw();

		return 0;
	});
}