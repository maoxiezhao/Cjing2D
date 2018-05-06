#include"timer.h"
#include"core\system.h"
#include"lua\luaContext.h"

Timer::Timer():
	mExpirationDate(System::Now()),
	mDuration(0),
	mFinished(true),
	mSuspensed(false),
	mSuspensedTime(0)
{
}

Timer::Timer(uint32_t duration):
	mExpirationDate(System::Now() + duration),
	mDuration(duration),
	mFinished((System::Now() >= mExpirationDate)),
	mSuspensed(false),
	mSuspensedTime(0)
{
}

void Timer::Update()
{
	if (mSuspensed || mFinished)
		return;
	if (System::Now() >= mExpirationDate)
		mFinished = true;
}

bool Timer::IsFinished() const
{
	return mFinished;
}

void Timer::SetFinished(bool finished)
{
	mFinished = finished;
}

bool Timer::IsSuspensed() const
{
	return mSuspensed;
}

/**
*	\biref ���ö�ʱ���ݶ�״̬
*
*	������ͣ����Ҫ������ͣʱ��ʧ��ʱ�䣬����ͣʱ����ʧʱ�䱣��
*	�ָ�ʱ�����ϵ�ǰʱ��
*/
void Timer::SetSuspensed(bool suspensed)
{
	if (suspensed != mSuspensed)
	{
		mSuspensed = suspensed;
		uint32_t now = System::Now();

		if (mSuspensed)
			mSuspensedTime = System::Now();
		else
		{	// �ս�ʱ��������ͣʱ��
			if (mSuspensedTime != 0)
				mExpirationDate += now - mSuspensedTime;
		}
	}
}

uint32_t Timer::GetExpirationDate() const
{
	return mExpirationDate;
}

uint32_t Timer::GetDuration() const
{
	return mDuration;
}

void Timer::SetExpirationDate(uint32_t expirationDate)
{
	mExpirationDate = expirationDate;
	mFinished = (System::Now() >= mExpirationDate) ;
}

void Timer::SetDuration(uint32_t duration)
{
	mDuration = duration;
	mExpirationDate = System::Now() + mDuration;
	mFinished = (System::Now() >= mExpirationDate);
}

const string Timer::GetLuaObjectName() const
{
	return LuaContext::module_time_name;
}
