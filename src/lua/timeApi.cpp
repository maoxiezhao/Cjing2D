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
*	\brief ���¶�ʱ��
*/
void LuaContext::UpdateTimers()
{
	// ����ÿ��timer
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
				CallTimerRef(timer);	// timer���ܻ��ظ�ִ�У��Ƿ�ɾ����
		}								// CallTimerRef���ж�
	}
	// ɾ��timer
	for (auto& timer : mTimersToRemove)
	{
		const auto& itr = mTimers.find(timer);
		if (itr != mTimers.end())
			mTimers.erase(itr);
	}
	mTimersToRemove.clear();
}

/**
*	\brief �ݻ����ж�ʱ��
*/
void LuaContext::DestoryTimers()
{
	mTimers.clear();
}

/**
*	\brief ɾ��ָ����ʱ��
*/
void LuaContext::RemoveTimer(TimerPtr & timer)
{
	mTimers[timer].removed = true;
	mTimersToRemove.push_back(timer);
}

/**
*	\brief ��Ӷ�ʱ��
*/
void LuaContext::AddTimer(const TimerPtr& timer, int contextIndex, const LuaRef& callback, bool repeat)
{
	const void* context = lua_topointer(l, contextIndex);
	// timer�����ظ�����
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
*	\brief ִ�ж�ʱ���ص�����
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
*	\brief C++ API.������ʱ��
*
*	Timer.Start(context,300,function() XXXX end) ����涨��ʱ������ֵΪ�Ƿ���ִ��
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

		// ����timer��timerdata
		TimerPtr timer = std::make_shared<Timer>(delay);
		luaContext.AddTimer(timer, 1, callBack, repeat);

		// ����Ѿ����(����ʱ��Ϊ0)��ֱ��ִ��callback
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