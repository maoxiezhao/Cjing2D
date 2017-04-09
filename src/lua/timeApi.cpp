#include"luaContext.h"
#include"luaRef.h"
#include"core\debug.h"

const string LuaContext::module_time_name = "Time";

void LuaContext::RegisterTimeModule()
{
	static const luaL_Reg functions[] = {
		{ "Start",time_api_start },
		{ nullptr,nullptr }
	};
	RegisterFunction(module_time_name, functions);

}

/**
*	\brief 更新定时器
*/
void LuaContext::UpdateTimers()
{
	// 更新每个timer
	for (const auto&kvp : mTimers)
	{
		auto timer = kvp.first;
		auto callBackRef = kvp.second.callBackRef;
		if(timer != nullptr && !callBackRef.IsEmpty())
		{
			timer->Update();
			if (timer->IsFinished())
				CallTimerRef(timer);	// timer可能会重复执行，是否删除在
		}								// CallTimerRef中判断
	}
	// 删除timer
	for (auto& timer : mTimersToRemove)
	{
		const auto& itr = mTimers.find(timer);
		if (itr != mTimers.end())
			mTimers.erase(itr);
	}
	mTimersToRemove.clear();
}

/**
*	\brief 摧毁所有定时器
*/
void LuaContext::DestoryTimers()
{
	for (auto& kvp : mTimers)
		mTimers.erase(kvp.first);
	mTimers.clear();
}

/**
*	\brief 添加定时器
*/
void LuaContext::AddTimer(const TimerPtr& timer, int contextIndex, const LuaRef& callback)
{
	const void* context = lua_topointer(l, contextIndex);
	// timer不可重复存入
	Debug::CheckAssertion(mTimers.find(timer) == mTimers.end(),
		"duplicate timers.");

	mTimers[timer].callBackRef = callback;
	mTimers[timer].l = context;

}

/**
*	\brief 执行定时器回调函数
*/
void LuaContext::CallTimerRef(const TimerPtr& timer)
{
	auto itr = mTimers.find(timer);
	if (itr != mTimers.end() && !itr->second.callBackRef.IsEmpty())
	{
		LuaRef& callback = itr->second.callBackRef;
		callback.Push();
		// call callback
		bool success = LuaTools::CallFunction(l, 0, 1, "TimeCallBack");
		bool repeat = false;
		if (success)
		{	// 检查是否重复执行
			repeat = lua_isboolean(l, -1) && lua_toboolean(l, -1);
			lua_pop(l, 1);
		}

		// 如果重复执行则重新设置timer，加入mTimers
		if (repeat)
		{
			timer->SetDuration(timer->GetDuration());
			if(timer->IsFinished())
				CallTimerRef(timer);
		}
		else 
		{	// clear
			callback.Clear();		
			mTimersToRemove.push_back(timer);
		}

	}
}

/**
*	\brief C++ API.创建定时器
*
*	Timer.Start(context,300,function() XXXX end) 这里规定定时器返回值为是否多次执行
*/
int LuaContext::time_api_start(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {

	LuaContext& luaContext = GetLuaContext(l);
	if (lua_type(l, 1) != LUA_TNUMBER)
	{
		if (lua_type(l, 1) != LUA_TUSERDATA && lua_type(l, 1) != LUA_TTABLE)
			// type error
			std::cout << "wrong type." << std::endl;
	}

	int delay = LuaTools::CheckInt(l, 2);
	const LuaRef& callBack = LuaTools::CheckFunction(l, 3);

	// 创建timer和timerdata
	TimerPtr timer = std::make_shared<Timer>(delay);
	luaContext.AddTimer(timer, 1, callBack);

	// 如果已经完成(可能时间为0)则直接执行callback
	if (timer->IsFinished())
		luaContext.CallTimerRef(timer);

	return 1;
	});
}