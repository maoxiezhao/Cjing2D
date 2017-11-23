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
		{"setColor", sprite_api_set_color},
		{"setBlend", sprite_api_set_blend},
		{"getBlend", sprite_api_get_blend},
		{"setOpacity", sprite_api_set_opacity},
		{"getOpacity", sprite_api_get_opacity},
		{"setAnchor", sprite_api_set_anchor},
		{"setOutLined", sprite_api_set_outLined },
		{"setBlinking", sprite_api_set_blinking },
		{"setFlipX", sprite_api_set_flip_x },
		{"setFlipY", sprite_api_set_flip_y },
		{"draw", sprite_api_draw},
		// ����ķ���Ӧ����drawapiʵ�֣�������sprtie,��δʵ��
		{"getPos", drawable_api_get_pos},	
		{"setPos", drawable_api_set_pos },		
		{"runMovement", drawable_api_run_movement },
		{"getMovment", drawable_api_get_movement },
		{"stopMovement", drawable_api_stop_movment },
		{nullptr, nullptr}
	};

	static const luaL_Reg metamethods[] = {
		{"__gc", drawable_meta_api_gc },
		{nullptr, nullptr}
	};
	RegisterType(l, module_sprite_name, function, methods, metamethods);
}

/**
*	\brief ��spriteѹջ
*/
void LuaContext::PushSprite(lua_State*l, Sprite& sprite)
{
	PushUserdata(l, sprite);
}

/**
*	\brief ���ջ��index�������Ƿ�Ϊsprite userdata
*	\return the sprite
*/
SpritePtr LuaContext::CheckSprite(lua_State*l, int index)
{
	return std::static_pointer_cast<Sprite>(CheckUserdata(l, index, module_sprite_name));
}

/**
*	\brief ����ջ��index�������Ƿ�sprite
*	\return true �����sprite
*/
bool LuaContext::IsSprite(lua_State*l, int index)
{
	return IsUserdata(l, index, module_sprite_name);
}

/**
*	\brief ʵ��cjing.Sprite.create()
*
*	�������2�д�������ķ���
*	1.cjing.Sprite.create(pathStr)   ����ָ����ͼƬ����
*	2.cjing.Sprite.create({r,g,b,a}) ����ָ����ɫ��ͼƬ��
*/
int LuaContext::sprite_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		if (lua_type(l, 1) != LUA_TTABLE && lua_type(l, 1) != LUA_TSTRING)
		{
			LuaTools::Error(l, "Invalid param in sprite::create.");
			return 0;
		}

		SpritePtr sprite = nullptr;
		if (lua_type(l, 1) == LUA_TSTRING)
		{
			const string spriteName = LuaTools::CheckString(l, 1);
			sprite = std::make_shared<Sprite>(spriteName);
		}
		else
		{
			const Color4B& color = LuaTools::CheckColor(l, 1);
			sprite = std::make_shared<Sprite>(color, Size(0, 0));
		}
		GetLuaContext(l).AddDrawable(sprite);
		PushSprite(l, *sprite);

		return 1;
	});
}

/**
*	\brief ʵ��sprite:SetSize(x,y)
*/
int LuaContext::sprite_api_set_size(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		int x = (int)LuaTools::CheckNumber(l, 2);
		int y = (int)LuaTools::CheckNumber(l, 3);

		sprite.SetSize(Size(x, y));

		return 0;
	});
}

/**
*	\brief ʵ��w,h = sprite:GetSize()
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
*	\brief ʵ��sprite:SetBlend(srcBlend, dstBlend)
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
*	\brief ʵ��sprite:GetBlend,���ν�srcBLend��dstBlendѹ��ջ
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
*	\brief ʵ��sprite:SetOpacity(alpha)
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
*	\brief ����͸����sprite:GetOpacity()
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

/**
*	\brief ʵ��cjing.Sprite:setColor({r,g,b,a})
*/
int LuaContext::sprite_api_set_color(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		const Color4B color = LuaTools::CheckColor(l, 2);
		sprite.SetColor(color);

		return 0;
	});
}

int LuaContext::sprite_api_set_anchor(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		float x = LuaTools::CheckFloat(l, 2);
		float y = LuaTools::CheckFloat(l, 3);
		sprite.SetAnchorFloat(x, y );

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Sprite:setOutLined(lineWidth)
*
*	��lineWidth����0ʱ��������ߣ�С�ڵ���0ʱ�ر����
*/
int LuaContext::sprite_api_set_outLined(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		float lineWidth = LuaTools::CheckFloat(l, 2);
		sprite.SetOutLine(lineWidth);

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Sprite:setBlinking(blinkDelay)
*/
int LuaContext::sprite_api_set_blinking(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		uint32_t blinkDelay = LuaTools::CheckInt(l, 2);
		sprite.SetBlinking(blinkDelay);

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Sprite:setFlipX(bool)
*/
int LuaContext::sprite_api_set_flip_x(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		bool fliped = LuaTools::CheckBoolean(l, 2);
		sprite.SetFlipX(fliped);

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Sprite:setFlipY(bool)
*/
int LuaContext::sprite_api_set_flip_y(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Sprite& sprite = *CheckSprite(l, 1);
		uint32_t blinkDelay = LuaTools::CheckInt(l, 2);
		bool fliped = LuaTools::CheckBoolean(l, 2);
		sprite.SetFlipY(fliped);

		return 0;
	});
}

