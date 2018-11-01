#include"core\perf.h"

namespace perf
{
namespace {
	GPUtimer gpuTimer;
}

void initGPUTimer()
{
	gpuTimer.supported = true;
	gpuTimer.cur = 0;
	gpuTimer.ret = 0;
	memset(gpuTimer.queries, 0, sizeof(unsigned int) * GPU_QUERY_COUNT);
}

void startGPUTimer()
{
	if (gpuTimer.supported)
	{
		glBeginQuery(GL_TIMEOUT_EXPIRED, gpuTimer.queries[gpuTimer.cur % GPU_QUERY_COUNT]);
		gpuTimer.cur++;
	}
}

int stopGPUTimer(float * times, int maxTimes)
{
	GLint available = 1;
	int n = 0;
	if (!gpuTimer.supported)
		return 0;

	glEndQuery(GL_TIME_ELAPSED);
	//while (available && gpuTimer.ret <= gpuTimer.cur) {
	//	glGetQueryObjectiv(gpuTimer.queries[gpuTimer.ret % GPU_QUERY_COUNT], GL_QUERY_RESULT_AVAILABLE, &available);
	//	if(gpu)
	//}
	return n;
}
}