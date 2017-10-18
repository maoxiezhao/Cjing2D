#include "random.h"
#include <random>
#include <ctime>

namespace Random {

namespace {
	std::mt19937 engine;
}

void Initialize()
{
	engine = std::mt19937((uint32_t)std::time(nullptr));
}

uint32_t GetRandomInt(uint32_t x)
{
	return GetRandomInt(0, x);
}

uint32_t GetRandomInt(uint32_t x, uint32_t y)
{
	std::uniform_int_distribution<uint32_t> dist{};
	using param_type = std::uniform_int_distribution<uint32_t>::param_type;
	return dist(engine, param_type{ x,y });

}

float GetRandomFloat(float x)
{
	return GetRandomFloat(0, x);
}

float GetRandomFloat(float x, float y)
{
	std::uniform_real_distribution<float> dist{};
	using param_type = std::uniform_real_distribution<float>::param_type;
	return dist(engine, param_type{ x,y });
}

double GetRandomDouble(double x, double y)
{
	std::uniform_real_distribution<double> dist{};
	using param_type = std::uniform_real_distribution<double>::param_type;
	return dist(engine, param_type{ x,y });
}

double GetRandomDouble(double x)
{
	return GetRandomDouble(0, x);
}

}