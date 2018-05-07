#include"lua\luaContext.h"
#include"core\debug.h"
#include"game\drawable.h"

const string LuaContext::module_drawable_name = "Drawable";

DrawablePtr LuaContext::CheckDrawable(lua_State*l, int index)
{
	if (IsDrawable(l, index))
	{
		const LuaObjectPtr& userdata = *(static_cast<LuaObjectPtr*>(lua_touserdata(l, index)));
		return std::static_pointer_cast<Drawable>(userdata);
	}
	else
	{
		LuaTools::ArgError(l, index, "Not a Drawable");
		throw;
	}
}

bool LuaContext::IsDrawable(lua_State*l, int index)
{
	return IsSprite(l, index) || 
		   IsAnimation(l, index) ||
		   IsTextDrawable(l, index) ||
		   IsParticle(l,index);
}

void LuaContext::PushDrawable(lua_State*l, Drawable& drawable)
{
	PushUserdata(l, drawable);
}

void LuaContext::AddDrawable(const std::shared_ptr<Drawable>& drawable)
{
	Debug::CheckAssertion(!HasDrawable(drawable), 
		"The drawalbe has already insert drawables.");

	mDrawables.insert(drawable);
}

bool LuaContext::HasDrawable(const std::shared_ptr<Drawable>& drawalbe)
{
	return mDrawables.find(drawalbe) != mDrawables.end();
}

void LuaContext::UpdateDrawables()
{
	for (const DrawablePtr& drawable : mDrawables)
	{
		if (HasDrawable(drawable))
		{
			drawable->Update();
		}
	}
	// É¾³ýÒªÒÆ³ýµÄdrawable
	for (const DrawablePtr& drawable: mDrawablesToRemove)
	{
		mDrawables.erase(drawable);
	}
	mDrawablesToRemove.clear();
}

void LuaContext::DestoryDrawables()
{
	mDrawables.clear();
	mDrawablesToRemove.clear();
}

void LuaContext::RemoveDrawable(const std::shared_ptr<Drawable>& drawable)
{
	Debug::CheckAssertion(HasDrawable(drawable),
		"The drawable have not created in RemoveDrawalbe.");
	mDrawablesToRemove.insert(drawable);
}

int LuaContext::drawable_api_get_pos(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		DrawablePtr drawable = CheckDrawable(l, 1);
		Point2 pos = drawable->GetPos();
		
		lua_pushinteger(l, pos.x);
		lua_pushinteger(l, pos.y);

		return 2;
	});
}

int LuaContext::drawable_api_set_pos(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		DrawablePtr drawable = CheckDrawable(l, 1);
		Point2 pos = LuaTools::CheckPoint2(l, 2);
		drawable->SetPos(pos);

		return 0;
	});
}

int LuaContext::drawable_meta_api_gc(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		DrawablePtr drawalbe = CheckDrawable(l, 1);

		if (luaContext.HasDrawable(drawalbe))
		{
			luaContext.RemoveDrawable(drawalbe);
		}
		userdata_meta_gc(l);

		return 0;
	});
}

int LuaContext::drawable_api_run_movement(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}

int LuaContext::drawable_api_get_movement(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}

int LuaContext::drawable_api_stop_movment(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {


		return 1;
	});
}
