#include "random.h"
#include <random>
#include <ctime>

uint32_t Random::GetRandomInt(uint32_t x)
{
	return GetRandomInt(0, x);
}

uint32_t Random::GetRandomInt(uint32_t x, uint32_t y)
{
	std::mt19937 engine((uint32_t)std::time(nullptr));
	std::uniform_int_distribution<uint32_t> dist{};

	using param_type = std::uniform_int_distribution<uint32_t>::param_type;
	return dist(engine, param_type{x,y});

}

float Random::GetRandomFloat(float x)
{
	return GetRandomFloat(0, x);
}

float Random::GetRandomFloat(float x, float y)
{
	std::mt19937 engine((uint32_t)std::time(nullptr));
	std::uniform_real_distribution<float> dist{};

	using param_type = std::uniform_real_distribution<float>::param_type;
	return dist(engine, param_type{ x,y});
}

