#pragma once

#include"common\common.h"

#include<memory>

struct NVGcontext;

/**
*	\brief Perf���ܷ�������Ҫ����CPUʱ���GPUʱ��
*/
#define GPU_QUERY_COUNT 5

namespace perf {

/**
*	\brief GPUʱ��ͳ��
*/
struct GPUtimer {
	bool supported;
	int cur, ret;
	unsigned int queries[GPU_QUERY_COUNT];

	GPUtimer() :supported(true), cur(0), ret(0)
	{
		memset(queries, 0, sizeof(unsigned int) * GPU_QUERY_COUNT);
	}
};
typedef struct GPUtimer GPUtimer;

void startGPUTimer();
int stopGPUTimer(float* times, int maxTimes);

}