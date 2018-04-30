#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"movements\movement.h"
#include"movements\targetMovement.h"
#include"movements\pathMovement.h"
#include"movements\pathFinding_movement.h"
#include"entity\entity.h"

const string LuaContext::module_movement_name = "Movement";
const string LuaContext::module_straight_movement_name = "StraightMovement";
const string LuaContext::module_target_movement_name = "TargetMovement";
const string LuaContext::module_path_movement_name = "PathMovement";
const string LuaContext::module_path_finding_movement_name = "PathFindingMovement";

void LuaContext::RegisterMovementModule()
{
	// base movement
	static const luaL_Reg methods[] = {
		{ "Start", movement_api_start},
		{ "Stop", movement_api_stop},
		{ "SetPos", movement_api_set_pos},
		{ "GetPos", movement_api_get_pos},
		{ nullptr, nullptr }
	};
	static const luaL_Reg metamethods[] = {
		{ "__gc", movement_api_api_gc },
		{ nullptr, nullptr }
	};
	RegisterType(l, module_movement_name, nullptr, methods, nullptr);

	// straight movement
	static const luaL_Reg straightFunctions[] = {
		{ "create", movement_straight_api_create},
		{nullptr, nullptr}
	};
	static const luaL_Reg straightMethods[] = {
		{ "setSpeed", movement_straight_api_set_speed},
		{ "getSpeed", movement_straight_api_get_speed},
		{ "setAngle", movement_straight_api_set_angle},
		{ "getAngle", movement_straight_api_get_angle},
		{ "setMaxDistance",movement_straight_api_set_max_distance},
		{ "getMaxDistance",movement_straight_api_get_max_distance},
		{nullptr, nullptr}
	};
	RegisterType(l, module_straight_movement_name, module_movement_name,
		straightFunctions, straightMethods, metamethods);

	// target movement
	static const luaL_Reg targetFunctions[] = {
		{ "create", movement_target_api_create },
		{ nullptr, nullptr }
	};
	static const luaL_Reg targetMethods[] = {
		{ "setSpeed", movement_target_api_set_speed },
		{ "getSpeed", movement_target_api_get_speed },
		{ "setTarget", movement_target_api_set_target},
		{ "getTarget", movement_target_api_get_target},
		{ nullptr, nullptr }
	};
	RegisterType(l, module_target_movement_name, module_straight_movement_name,
		targetFunctions, targetMethods, metamethods);

	// path movement
	LuaBindClass<PathMovement> pathMovementClass(l, module_path_movement_name, module_movement_name);
	pathMovementClass.AddDefaultMetaFunction();
	pathMovementClass.AddFunction("Create", movement_path_pai_create);
	pathMovementClass.AddMethod("SetPath", &PathMovement::SetPathString);
	pathMovementClass.AddMethod("SetSpeed", &PathMovement::SetSpeed);

	// path finding movement
	LuaBindClass<PathFindingMovement> pathFindingClass(l, module_path_finding_movement_name, module_path_movement_name);
	pathMovementClass.AddDefaultMetaFunction();
}

/************************************************************
*	\brief movement
************************************************************/

std::shared_ptr<Movement> LuaContext::CheckMovement(lua_State*l, int index)
{
	return std::static_pointer_cast<Movement>(CheckUserdata(l, index, module_movement_name));
}

/**
*	\brief 检测是否是movement
*
*	判断是否是所有可能的Movement的派生类
*/
bool LuaContext::IsMovement(lua_State*l, int index)
{
	return IsStraightMovement(l, index) ||
		IsTargetMovement(l, index);
}

void LuaContext::PushMovement(lua_State*l, Movement& movement)
{
	PushUserdata(l, movement);
}

int LuaContext::movement_api_api_gc(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		userdata_meta_gc(l);

		return 0;
	});
}

/**
*	\brief 实现movement:start()
*
*    movement:start(drawable || entity , function() end)
*/
int LuaContext::movement_api_start(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		std::shared_ptr<Movement> movement = CheckMovement(l, 1);
		movement->Stop();
		// 移动对象
		if (IsDrawable(l, 2))
		{
			Drawable& drawable = *CheckDrawable(l, 2);
			drawable.StartMovement(movement);
		}
		else if (IsEntity(l, 2)) 
		{
			Entity& entity = *CheckEntity(l, 2);
			entity.StartMovement(movement);
		}
		else
		{
			LuaTools::ArgError(l, 2, "movement:start except table,entity,or drawable.");
		}
		// 结束回调
		LuaRef callBack = LuaTools::OptFunction(l, 3);
		movement->SetFinishedCallBack(callBack);

		return 0;
	});
}

/**
*	\brief 实现movement:stop()
*/
int LuaContext::movement_api_stop(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		std::shared_ptr<Movement> movement = CheckMovement(l, 1);
		
		Entity* entity = movement->GetEntity();
		if (entity != nullptr)
		{
			entity->ClearMovements();
		}
		else
		{
			Drawable* drawable = movement->GetDrawable();
			if (drawable != nullptr)
			{
				drawable->StopMovement();
			}
			else
			{
				movement->Stop();
			}
		}
		return 0;
	});
}

int LuaContext::movement_api_set_pos(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Movement& movement = *CheckMovement(l, 1);
		int x = LuaTools::CheckInt(l, 2);
		int y = LuaTools::CheckInt(l, 3);
		movement.SetPos(Point2(x,y));

		return 0;
	});
}

int LuaContext::movement_api_get_pos(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Movement& movement = *CheckMovement(l, 1);
		const Point2& pos = movement.GetPos();
		lua_pushinteger(l, pos.x);
		lua_pushinteger(l, pos.y);

		return 2;
	});
}

/************************************************************
*	\brief straight movement
************************************************************/

std::shared_ptr<StraightMovement> LuaContext::CheckStraightMovement(lua_State * l, int index)
{
	return std::static_pointer_cast<StraightMovement>(CheckUserdata(l, index, module_straight_movement_name));
}

bool LuaContext::IsStraightMovement(lua_State * l, int index)
{
	return IsUserdata(l, index, module_straight_movement_name);
}

int LuaContext::movement_straight_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		bool ingroedObstacled = LuaTools::OptBoolean(l, 1, false);
		std::shared_ptr<StraightMovement> movement = std::make_shared<StraightMovement>(ingroedObstacled);
		movement->SetSpeed(8);

		PushMovement(l, *movement);
		return 1;
	});	
}

int LuaContext::movement_straight_api_set_speed(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		int speed = LuaTools::CheckInt(l, 2);
		movement.SetSpeed((double)speed);

		return 0;
	});
}

int LuaContext::movement_straight_api_get_speed(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		int speed = (int)movement.GetSpeed();
		lua_pushinteger(l, speed);

		return 1;
	});
}

int LuaContext::movement_straight_api_set_angle(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		double angle = LuaTools::CheckNumber(l, 2);
		movement.SetAngle(angle);

		return 0;
	});
}

int LuaContext::movement_straight_api_get_angle(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		double angle = movement.GetSpeed();
		lua_pushnumber(l, angle);

		return 1;
	});
}

int LuaContext::movement_straight_api_set_max_distance(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		double distance = LuaTools::CheckNumber(l, 2);
		movement.SetMaxDistance(distance);

		return 0;
	});
}

int LuaContext::movement_straight_api_get_max_distance(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		StraightMovement& movement = *CheckStraightMovement(l, 1);
		double distance = movement.GetMaxDistance();
		lua_pushnumber(l, distance);

		return 1;
	});
}

/************************************************************
*	\brief target movement
************************************************************/

std::shared_ptr<TargetMovement> LuaContext::CheckTargetMovement(lua_State * l, int index)
{
	return std::static_pointer_cast<TargetMovement>(CheckUserdata(l, index, module_target_movement_name));
}

bool LuaContext::IsTargetMovement(lua_State * l, int index)
{
	return IsUserdata(l, index, module_target_movement_name);
}

int LuaContext::movement_target_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		std::shared_ptr<TargetMovement> targetMovement =
			std::make_shared<TargetMovement>(nullptr, Point2(0,0), 8, false);
		PushMovement(l, *targetMovement);

		return 1;
	});
}

int LuaContext::movement_target_api_set_target(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TargetMovement& targetMovement = *CheckTargetMovement(l, 1);
		int x = (int)LuaTools::CheckNumber(l, 2);
		int y = (int)LuaTools::CheckNumber(l, 3);
		targetMovement.SetTarget(nullptr, Point2(x, y));

		return 0;
	});
}

int LuaContext::movement_target_api_get_target(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		
		return 0;
	});
}

int LuaContext::movement_target_api_set_speed(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TargetMovement& movement = *CheckTargetMovement(l, 1);
		int speed = LuaTools::CheckInt(l, 2);
		movement.SetMovingSpeed(speed);

		return 0;
	});
}

int LuaContext::movement_target_api_get_speed(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TargetMovement& movement = *CheckTargetMovement(l, 1);
		int speed = movement.GetMovingSpeed();
		lua_pushinteger(l, speed);

		return 1;
	});
}

/**-----------------------------------------------------
*	\brief PathMovement
*///----------------------------------------------------

int LuaContext::movement_path_pai_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& path = LuaTools::CheckString(l, 1);
		int speed = LuaTools::CheckInt(l, 2);
		bool loop = LuaTools::OptBoolean(l, 3, false);
		bool ingore = LuaTools::OptBoolean(l, 4, false);
		std::shared_ptr<PathMovement> movement =
			std::make_shared<PathMovement>(path, speed, loop, ingore);

		PushMovement(l, *movement);
		return 1;
	});
}
