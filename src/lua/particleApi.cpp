#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"game\particleSystem.h"

const string LuaContext::module_particle_name = "Particle";

/**
*	\brief 注册Particle模块
*/
void LuaContext::RegisterParticle()
{
	static const luaL_Reg methods[] = {
		{ "play", particle_api_play},
		{ "stop", particle_api_stop},
		{ "setPreprocess", particle_api_set_preprocess },
		{ "draw", particle_api_draw },
		{ "getPos", drawable_api_get_pos },		// 下面的方法应该在drawapi实现，派生给sprtie,暂未实现
		{ "setPos", drawable_api_set_pos },
		{ "runMovement", drawable_api_run_movement },
		{ "getMovment", drawable_api_get_movement },
		{ "stopMovement", drawable_api_stop_movment },
		{ nullptr, nullptr }
	};

	LuaBindClass<ParticleSystem> particleSystem(l, module_particle_name);
	particleSystem.AddMetaFunction("__gc", drawable_meta_api_gc);
	particleSystem.AddFunction("Create", particle_api_create);
	particleSystem.AddMethod("Play", particle_api_play);
	particleSystem.AddMethod("Stop", particle_api_stop);
	particleSystem.AddMethod("SetPreprocess", particle_api_set_preprocess);
}

/**
*	\brief 将ParticleSystem压栈
*/
void LuaContext::PushParticle(lua_State*l, ParticleSystem& particle)
{
	PushUserdata(l, particle);
}

/**
*	\brief 检查栈上index索引处是否为ParticleSystem userdata
*	\return ParticleSystemPtr
*/
std::shared_ptr<ParticleSystem> LuaContext::CheckParticle(lua_State*l, int index)
{
	return std::dynamic_pointer_cast<ParticleSystem>(CheckUserdata(l, index, module_particle_name));
}

/**
*	\brief 返回栈上index索引处是否ParticleSystem
*	\return true 如果是ParticleSystem
*/
bool LuaContext::IsParticle(lua_State*l, int index)
{
	return IsUserdata(l, index, module_particle_name);
}

/**
*	\brief 实现cjing.Particle.create()
*/
int LuaContext::particle_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& particleFile = LuaTools::CheckString(l, 1);
		std::shared_ptr<ParticleSystem> particle = std::make_shared<ParticleSystem>(particleFile);
		particle->Initialize();

		GetLuaContext(l).AddDrawable(particle);
		PushParticle(l, *particle);

		return 1;
	});
}

/**
*	\brief 实现Particle:Play(x, y, time)
*/
int LuaContext::particle_api_play(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		ParticleSystem& particle = *CheckParticle(l, 1);
		int x = LuaTools::CheckInt(l, 2);
		int y = LuaTools::CheckInt(l, 3);
		int time = LuaTools::CheckIntByDefault(l, 4, 0);

		particle.SetPos({ x, y });
		particle.Start(static_cast<uint32_t>(time));

		return 0;
	});
}

/**
*	\brief 实现Particle:Stop()
*/
int LuaContext::particle_api_stop(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		ParticleSystem& particle = *CheckParticle(l, 1);
		particle.Stop();

		return 0;
	});
}

/**
*	\brief 实现Particle:setPreproces(function(drawable) end)
*/
int LuaContext::particle_api_set_preprocess(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		ParticleSystem& particle = *CheckParticle(l, 1);
		LuaRef callBack = LuaTools::CheckFunction(l, 2);
		particle.SetPreProcess(callBack);

		return 0;
	});
}

/**
*	\brief 实现Particle:draw()
*/
int LuaContext::particle_api_draw(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		ParticleSystem& particle = *CheckParticle(l, 1);
		particle.Draw();
		return 0;
	});
}

