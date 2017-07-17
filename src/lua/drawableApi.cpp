#include"lua\luaContext.h"
#include"core\debug.h"
#include"game\drawable.h"

const string LuaContext::module_drawable_name = "Drawable";


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
