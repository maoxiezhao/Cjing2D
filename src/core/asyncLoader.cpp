#include "asyncLoader.h"
#include "debug.h"
#include "logger.h"
#include <asyncinfo.h>

AsyncLoader::AsyncLoader():
	mFinishCallBack(),
	mTaskQueue(),
	mMaxTimeLimt(0),
	mUpdateStep(0),
	mUsedTime(0),
	mStartd(false),
	mFinished(false)
{
}

void AsyncLoader::Update()
{
	if (!mStartd || mFinished)
	{
		return;
	}
	std::future_status status = mFuture.wait_for(std::chrono::milliseconds(mUpdateStep / 2));
	if (status == std::future_status::ready)
	{
		//Logger::Info("Async load finished.");
		mFinished = true;
		if (mFinishCallBack)
		{
			mFinishCallBack();
		}
	}
	mUsedTime += mUpdateStep;
	if (mMaxTimeLimt > 0 && mUsedTime >= mMaxTimeLimt)
	{
		Debug::Die("Time out to async load.");
	}
}

void AsyncLoader::Run(uint32_t maxTimeLimt)
{
	if (mFinished)
	{
		return;
	}
	Logger::Info("Async loading start.");
	mStartd = true;
	mMaxTimeLimt = maxTimeLimt;
	mFuture = std::async(std::launch::async, [&]() {
		auto& taskQueue = this->GetTaksQueue();
		while (!taskQueue.empty())
		{
			auto task = taskQueue.front();
			taskQueue.pop();
			task();
		}
	});
}

void AsyncLoader::AddTask(const TaskFunction & task)
{
	mTaskQueue.push(task);
}
