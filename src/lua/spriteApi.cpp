#include"lua\luaContext.h"
#include"lua\luaTools.h"
#include"game\sprite.h"
#include"game\enumInfo.h"

const string LuaContext::module_sprite_name = "Sprite";

void LuaContext::RegisterSpriteModule()
{
	static const luaL_Reg function[] = {
		{"create", sprite_api_create},
		{nullptr,nullptr}
	};

	static const luaL_Reg methods[] = {
		{"setSize", sprite_api_set_size},
		{"getSize", sprite_api_get_size},
		{"setRotation", sprite_api_set_rotation},
		{"getRotation", sprite_api_get_rotation},
		{"setBlend", sprite_api_set_blend},
		{"getBlend", sprite_api_get_blend},
		{"setOpacity", sprite_api_set_opacity},
		{"getOpacity", sprite_api_get_opacity},
		{"draw", sprite_api_draw},
		{"getPos", drawable_api_get_pos},
		{"setPos", drawable_api_set_pos },		// 下面的方法应该在drawapi实现，派生给sprtie,暂未实现
		{"runMovement", sprite_api_run_movement },
		{"getMovment", sprite_api_get_movement },
		{"stopMovement", sprite_api_stop_movment },
		{nullptr, nullptr}
	};

	static const luaL_Reg metamethods[] = {
		{"__gc", drawable_meta_api_gc },
		{nullptr, nullptr}
	};
	RegisterType(module_sprite_name, function, methods, metamethods);
}

/**
*	\brief 将sprite压栈
*/
void LuaContext::PushSprite(lua_State*l, Sprite& sprite)
{
	PushUserdata(l, sprite);
}

/**
*	\brief 检查栈上index索引处是否为sprite userdata
*	\return the sprite
*/
SpritePtr LuaContext::CheckSprite(lua_State*l, int index)
{
	return std::static_pointer_cast<Sprite>(CheckUserdata(l, index, module_sprite_name));
}

/**
*	\brief 返回栈上index索引处是否sprite
*	\return true 如果是sprite
*/
bool LuaContext::IsSprite(lua_State*l, int index)
{
	return IsUserdata(l, index, module_sprite_name);
}

/**
*	\brief 实现cjing.Sprite.create()
*
*	这里存在2中创建精灵的方法,目前考虑是否支持第二种纯色精灵的创建
*/
int LuaContext::sprite_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string spriteName = LuaTools::CheckString(l, 1);
		
		SpritePtr sprite = std::make_shared<Sprite>(spriteName);
		GetLuaContext(l).AddDrawable(sprite);

		PushSprite(l, *sprite);
		return 1;
	});
}

/**
*	\brief 实现sprite:SetSize(x,y)
*/
int LuaContext::sprite_api_set_size(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		int x = LuaTools::CheckInt(l, 2);
		int y = LuaTools::CheckInt(l, 3);

		sprite.SetSize(Size(x, y));

		return 0;
	});
}

/**
*	\brief 实现w,h = sprite:GetSize()
*/
int LuaContext::sprite_api_get_size(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);

		const Size size = sprite.GetSize();
		lua_pushinteger(l, size.width);
		lua_pushinteger(l, size.height);

		return 2;
	});
}

/**
*	\brief 实现sprite:SetBlend(srcBlend, dstBlend)
*/
int LuaContext::sprite_api_set_blend(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		GLenum srcBlend = LuaTools::CheckEnum<GLenum>(l, 2);
		GLenum dstBlend = LuaTools::CheckEnum<GLenum>(l, 3);

		sprite.SetBlendFunc(BlendFunc(srcBlend, dstBlend));

		return 0;
	});
}

/**
*	\brief 实现sprite:GetBlend,依次将srcBLend和dstBlend压入栈
*/
int LuaContext::sprite_api_get_blend(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		BlendFunc blend = sprite.GetBlendFunc();

		lua_pushstring(l, EnumToString(blend.srcBlend).c_str());
		lua_pushstring(l, EnumToString(blend.dstBlend).c_str());

		return 2;
	});
}

/**
*	\brief 实现sprite:SetOpacity(alpha)
*/
int LuaContext::sprite_api_set_opacity(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		int alpha = LuaTools::CheckInt(l, 2);
		sprite.SetOpacity(alpha);

		return 0;
	});
}

/**
*	\brief 返回透明度sprite:GetOpacity()
*/
int LuaContext::sprite_api_get_opacity(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		int alpha = sprite.GetOpacity();
		lua_pushinteger(l, alpha);

		return 1;
	});
}

int LuaContext::sprite_api_draw(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		/*int x = LuaTools::CheckInt(l, 2);
		int y = LuaTools::CheckInt(l, 3);*/
		sprite.Draw();

		return 0;
	});
}

int LuaContext::sprite_api_run_movement(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}

int LuaContext::sprite_api_get_movement(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}

int LuaContext::sprite_api_stop_movment(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}

int LuaContext::sprite_api_set_rotation(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		float angle = LuaTools::CheckFloat(l, 2);
		sprite.SetRotated(angle);

		return 0;
	});
}

int LuaContext::sprite_api_get_rotation(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		float angle = sprite.GetRotated();
		lua_pushnumber(l, angle);

		return 1;
	});
}