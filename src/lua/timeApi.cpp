#include"lua\luaContext.h"
#include"lua\luaRef.h"
#include"lua\luaBinder.h"
#include"core\debug.h"
#include"core\system.h"

const string LuaContext::module_time_name = "Timer";

void LuaContext::RegisterTimeModule()
{
	LuaBindClass<Timer> timerClass(l, module_time_name);
	timerClass.AddDefaultMetaFunction();
	timerClass.AddFunction("Start", time_api_start);
	timerClass.AddFunction("Remove", time_api_remove);
	timerClass.AddFunction("Clear", time_api_clear);
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
		auto& info = kvp.second;
		auto callBackRef = info.callBackRef;

		if(timer != nullptr && 
			!callBackRef.IsEmpty() && 
			!info.removed)
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
	mTimers.clear();
}

/**
*	\brief 删除指定定时器
*/
void LuaContext::RemoveTimer(TimerPtr & timer)
{
	mTimers[timer].removed = true;
	mTimersToRemove.push_back(timer);
}

/**
*	\brief 添加定时器
*/
void LuaContext::AddTimer(const TimerPtr& timer, int contextIndex, const LuaRef& callback, bool repeat)
{
	const void* context = lua_topointer(l, contextIndex);
	// timer不可重复存入
	Debug::CheckAssertion(mTimers.find(timer) == mTimers.end(),
		"duplicate timers.");

	TimerData timerData;
	timerData.callBackRef = callback;
	timerData.l = context;
	timerData.repeat = repeat;
	timerData.removed = false;

	mTimers[timer] = timerData;
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
		LuaTools::CallFunction(l, 0, 0, "TimeCallBack");
	
		bool repeat = itr->second.repeat;
		if (repeat)
		{
			timer->SetDuration(timer->GetDuration());
			if(timer->IsFinished())
				CallTimerRef(timer);
		}
		else 
		{	
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
				LuaTools::Error(l, "wrong type.");
		}

		int delay = LuaTools::CheckInt(l, 2);
		const LuaRef& callBack = LuaTools::CheckFunction(l, 3);
		bool repeat = LuaTools::CheckBoolean(l, 4);

		// 创建timer和timerdata
		TimerPtr timer = std::make_shared<Timer>(delay);
		luaContext.AddTimer(timer, 1, callBack, repeat);

		// 如果已经完成(可能时间为0)则直接执行callback
		if (timer->IsFinished())
			luaContext.CallTimerRef(timer);

		luaContext.PushUserdata(l, *timer);
		return 1;
	});
}

/**
*	\brief Timer.Remove(timer)
*/
int LuaContext::time_api_remove(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto timer = std::static_pointer_cast<Timer>(
			CheckUserdata(l, 1, module_time_name));

		GetLuaContext(l).RemoveTimer(timer);
		return 0;
	});
}

/**
*	\brief Timer.Clear()
*/
int LuaContext::time_api_clear(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		GetLuaContext(l).DestoryTimers();
		return 0;
	});
}