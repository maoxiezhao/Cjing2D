#ifndef _TIMER_H_
#define _TIMER_H_

#include"common\common.h"

/**
*	\brief ¶¨Ê±Æ÷
*/
class Timer
{
public:
	Timer();
	Timer(uint32_t duration);

	void Update();
	bool IsFinished()const;
	void SetFinished(bool finished);
	bool IsSuspensed()const;
	void SetSuspensed(bool suspensed);

	uint32_t GetExpirationDate()const;
	uint32_t GetDuration()const;
	void SetExpirationDate(uint32_t expirationDate);
	void SetDuration(uint32_t duration);

private:
	uint32_t mExpirationDate;
	uint32_t mDuration;
	uint32_t mSuspensedTime;
	bool mFinished;
	bool mSuspensed;

};

using TimerPtr = std::shared_ptr<Timer>;

#endif