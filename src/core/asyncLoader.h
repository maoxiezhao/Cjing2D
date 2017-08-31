#pragma once

#include"common\common.h"
#include"core\font.h"
#include"core\fontAtlas.h"
#include"core\resourceCache.h"
#include"game\timer.h"
#include"game\currentQuest.h"
#include"lua\luaObject.h"
#include"lua\luaRef.h"

#include<thread>
#include<functional>
#include<queue>
#include<future>

/**
*	\brief 创建一个线程用于异步加载需要的资源
*
*	目前考虑创建一个任务队列，当任务队列完成时，会执行完成回调函数
*	暂时不需要考虑互斥操作，因为约定好在外部等待加载的时候不会使用
*	相同的资源
*
*	异步加载资源,可异步的资源为字体、除default之外的纹理、音频、动
*	画，目前异步加载动作在lua中处理,所有的lua函数以taskfunction包装
*	统一处理
*
*	当等待时间过长时，应执行对应的回到函数
*/

class AsyncLoader : public LuaObject
{
	using TaskFunction = std::function<void()>;
public:
	AsyncLoader();
	~AsyncLoader();

	void Stop();
	void Update();
	void Run(uint32_t maxTimeLimt = 0);
	
	void AddTask(const TaskFunction& task);
	void AddTask(const LuaRef& callback);

	void SetFinishCallBack(const std::function<void()>& finisCallBack);
	void SetFinishCallBack(const LuaRef& callback);

	void ClearTaskQueue();

	virtual const string GetLuaObjectName()const;

	/**** **** setter/gettter **** ****/
	bool IsTaskListEmpty()const
	{
		return mTaskQueue.empty();
	}
	bool IsStarted()const
	{
		return mStartd;
	}

	bool IsFinished()const
	{
		return mFinished;
	}

	std::queue<TaskFunction>& GetTaksQueue()
	{
		return mTaskQueue;
	}

	void SetUpdateStep(uint32_t step)
	{
		mUpdateStep = step;
	}

	HGLRC GetHGLRC() const
	{
		return mHRCShareing;
	}

	HDC GetHDC() const
	{
		return mHDC;
	}

private:
	std::function<void()> mFinishCallBack;		/** 结束回调函数 */

	std::queue<TaskFunction> mTaskQueue;		/** 任务队列，仅当任务队列为空时执行结束回调函数 */

	uint32_t mMaxTimeLimt;		/** 最大执行时间 */
	uint32_t mUpdateStep;
	uint32_t mUsedTime;

	bool mStartd;				/** 是否已经开始标记 */			
	bool mFinished;
	std::future<void> mFuture;

	HGLRC mHRCShareing;			/** opengl多线程下的共享渲染上下文 */
	HDC   mHDC;
};