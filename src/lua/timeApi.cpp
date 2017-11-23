#include"luaContext.h"
#include"luaRef.h"
#include"core\debug.h"
#include"core\system.h"
const string LuaContext::module_time_name = "Timer";

void LuaContext::RegisterTimeModule()
{
	static const luaL_Reg functions[] = {
		{ "start",time_api_start },
		{ nullptr,nullptr }
	};
	RegisterFunction(l, module_time_name, functions);
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
		auto callBackRef = kvp.second.callBackRef;
		if(timer != nullptr && !callBackRef.IsEmpty())
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
	//for (auto& kvp : mTimers)
	//	mTimers.erase(kvp.first);
	mTimers.clear();
}

/**
*	\brief ��Ӷ�ʱ��
*/
void LuaContext::AddTimer(const TimerPtr& timer, int contextIndex, const LuaRef& callback)
{
	const void* context = lua_topointer(l, contextIndex);
	// timer�����ظ�����
	Debug::CheckAssertion(mTimers.find(timer) == mTimers.end(),
		"duplicate timers.");

	mTimers[timer].callBackRef = callback;
	mTimers[timer].l = context;

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
		// call callback
		bool success = LuaTools::CallFunction(l, 0, 1, "TimeCallBack");
		bool repeat = false;
		if (success)
		{	// ����Ƿ��ظ�ִ��
			repeat = lua_isboolean(l, -1) && lua_toboolean(l, -1);
			lua_pop(l, 1);
		}

		// ����ظ�ִ������������timer������mTimers
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
*	\brief C++ API.������ʱ��
*
*	Timer.Start(context,300,function() XXXX end) ����涨��ʱ������ֵΪ�Ƿ���ִ��
*/
int LuaContext::time_api_start(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {

	LuaContext& luaContext = GetLuaContext(l);
	//luaContext.PrintLuaStack(l);

	if (lua_type(l, 1) != LUA_TNUMBER)
	{
		if (lua_type(l, 1) != LUA_TUSERDATA && lua_type(l, 1) != LUA_TTABLE)
			// type error
			LuaTools::Error(l, "wrong type.");

	}

	int delay = LuaTools::CheckInt(l, 2);
	const LuaRef& callBack = LuaTools::CheckFunction(l, 3);

	// ����timer��timerdata
	TimerPtr timer = std::make_shared<Timer>(delay);
	luaContext.AddTimer(timer, 1, callBack);

	// ����Ѿ����(����ʱ��Ϊ0)��ֱ��ִ��callback
	if (timer->IsFinished())
		luaContext.CallTimerRef(timer);

	return 1;
	});
}