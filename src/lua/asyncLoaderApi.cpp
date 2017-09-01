#include"lua\luaContext.h"
#include"core\asyncLoader.h"

const string LuaContext::module_async_loader_name = "AsyncLoader";

void LuaContext::RegisterAsyncLoaderModule()
{
	static const luaL_Reg function[] = {
		{ "create", async_loader_api_create },
		{ nullptr,nullptr }
	};

	static const luaL_Reg methods[] = {
		{"addTask", async_loader_api_add_task},
		{"setFinishedCallBack", async_loader_api_set_callback},
		{"run", async_loader_api_run},
		{ nullptr,nullptr }
	};

	static const luaL_Reg metamethods[] = {
		{ "__gc", async_loader_meta_api_gc },
		{ nullptr, nullptr }
	};

	RegisterType(module_async_loader_name, function, methods, metamethods);
}

void LuaContext::UpdateAsyncLoaders()
{
	for (const auto& loader : mAsyncLoaders)
	{
		loader->Update();
		//if (loader->IsFinished())
		//	mAsyncLoaderToRemove.insert(loader);
	}

	for (const std::shared_ptr<AsyncLoader>& loader : mAsyncLoaderToRemove)
	{
		mAsyncLoaders.erase(loader);
	}
	mAsyncLoaderToRemove.clear();
}

void LuaContext::DestoryAsyncLoaders()
{
	mAsyncLoaders.clear();
	mAsyncLoaderToRemove.clear();
}

void LuaContext::AddAsyncLoaders(const std::shared_ptr<AsyncLoader>& asyncLoader)
{
	Debug::CheckAssertion(!HasAsyncLoader(asyncLoader),
		"The asyncloader has already insert asyncloaders.");
	mAsyncLoaders.insert(asyncLoader);
}

bool LuaContext::HasAsyncLoader(const std::shared_ptr<AsyncLoader>& asyncLoader)
{
	return mAsyncLoaders.find(asyncLoader) != mAsyncLoaders.end();
}

void LuaContext::RemoveAsyncLoader(const std::shared_ptr<AsyncLoader>& asyncLoader)
{
	Debug::CheckAssertion(HasAsyncLoader(asyncLoader),
		"The asyncloader to remove has not insert asyncloaders.");
	mAsyncLoaderToRemove.insert(asyncLoader);
}

void LuaContext::PushAsyncLoader(lua_State*l, AsyncLoader& asyncLoader)
{
	PushUserdata(l, asyncLoader);
}

std::shared_ptr<AsyncLoader> LuaContext::CheckAsyncLoader(lua_State*l, int index)
{
	return std::static_pointer_cast<AsyncLoader>(CheckUserdata(l, index, module_async_loader_name));
}

bool LuaContext::IsAsyncLoader(lua_State*l, int index)
{
	return IsUserdata(l, index, module_async_loader_name);
}

/**
*	\brief 实现AsyncLoader.create()
*
*	创建一个异步加载器
*/
int LuaContext::async_loader_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {

		std::shared_ptr<AsyncLoader> asyncLoader = std::make_shared<AsyncLoader>();
		GetLuaContext(l).AddAsyncLoaders(asyncLoader);

		PushAsyncLoader(l, *asyncLoader);
		return 1;
	});
}

/**
*	\brief 实现AsyncLoader:addTask(function)
*/
int LuaContext::async_loader_api_add_task(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AsyncLoader& asyncLoader = *CheckAsyncLoader(l, 1);
		LuaRef callBack = LuaTools::CheckFunction(l, 2);
		asyncLoader.AddTask(callBack);

		return 0;
	});
}

/**
*	\brief 实现AsyncLoader:setFinishedCallBack(function)
*/
int LuaContext::async_loader_api_set_callback(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AsyncLoader& asyncLoader = *CheckAsyncLoader(l, 1);
		LuaRef callBack = LuaTools::CheckFunction(l, 2);
		asyncLoader.SetFinishCallBack(callBack);

		return 0;
	});
}

/**
*	\brief 实现AsyncLoader:run()
*
*	默认不限制最大等待时间
*/
int LuaContext::async_loader_api_run(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		AsyncLoader& asyncLoader = *CheckAsyncLoader(l, 1);
		asyncLoader.Run();

		return 0;
	});
}

int LuaContext::async_loader_meta_api_gc(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		std::shared_ptr<AsyncLoader> asyncLoader = CheckAsyncLoader(l, 1);

		if (luaContext.HasAsyncLoader(asyncLoader))
		{
			luaContext.RemoveAsyncLoader(asyncLoader);
		}
		userdata_meta_gc(l);

		return 0;
	});
}