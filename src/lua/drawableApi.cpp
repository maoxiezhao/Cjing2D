#include"lua\luaContext.h"
#include"core\debug.h"

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

void LuaContext::RemoveDrawable(const std::shared_ptr<Drawable>& drawable)
{
	Debug::CheckAssertion(HasDrawable(drawable),
		"The drawalbe has not insert in drawalbes.");

	mDrawables.erase(drawable);
}

