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
	LuaBindClass<Movement> movementBaseCalss(l, module_movement_name);
	movementBaseCalss.AddDefaultMetaFunction();
	movementBaseCalss.AddMethod("Start", movement_api_start);
	movementBaseCalss.AddMethod("Stop", movement_api_stop);
	movementBaseCalss.AddMethod("SetPos", &Movement::SetPos);
	movementBaseCalss.AddMethod("GetPos", &Movement::GetPos);
	movementBaseCalss.AddMethod("GetDirection", &Movement::GetDirection);

	// straight movement
	LuaBindClass<StraightMovement> straightMovementClass(l, module_straight_movement_name, module_movement_name);
	straightMovementClass.AddDefaultMetaFunction();
	straightMovementClass.AddFunction("Create", movement_straight_api_create);
	straightMovementClass.AddMethod("SetSpeed", &StraightMovement::SetSpeed);
	straightMovementClass.AddMethod("GetSpeed", &StraightMovement::GetSpeed);
	straightMovementClass.AddMethod("SetAngle", &StraightMovement::SetAngle);
	straightMovementClass.AddMethod("GetAngle", &StraightMovement::GetAngle);
	straightMovementClass.AddMethod("SetMaxDistance", &StraightMovement::SetMaxDistance);
	straightMovementClass.AddMethod("GetMaxDistance", &StraightMovement::GetMaxDistance);

	// target movement
	LuaBindClass<TargetMovement> targetMovementClass(l, module_target_movement_name, module_straight_movement_name);
	targetMovementClass.AddDefaultMetaFunction();
	targetMovementClass.AddFunction("Create", movement_target_api_create);
	targetMovementClass.AddMethod("SetSpeed", movement_target_api_set_speed);
	targetMovementClass.AddMethod("GetSpeed", movement_target_api_get_speed);
	targetMovementClass.AddMethod("SetTarget", movement_target_api_set_target);
	targetMovementClass.AddMethod("GetTarget", movement_target_api_get_target);

	// path movement
	LuaBindClass<PathMovement> pathMovementClass(l, module_path_movement_name, module_movement_name);
	pathMovementClass.AddDefaultMetaFunction();
	pathMovementClass.AddFunction("Create", movement_path_pai_create);
	pathMovementClass.AddMethod("SetPath", &PathMovement::SetPathString);
	pathMovementClass.AddMethod("SetSpeed", &PathMovement::SetSpeed);

	// path finding movement
	LuaBindClass<PathFindingMovement> pathFindingClass(l, module_path_finding_movement_name, module_path_movement_name);
	pathFindingClass.AddDefaultMetaFunction();
	pathFindingClass.AddFunction("Create", movement_path_finding_api_create);
	pathFindingClass.AddMethod("SetTarget", movement_path_finding_api_set_target);
}

/************************************************************
*	\brief movement
************************************************************/

void LuaContext::NotifyEntityWithMovement(Entity& entity, const std::string& funcName)
{
	CallFunctionWithUserdata(entity, funcName,
		[&](lua_State* l)->int {
		auto& movement = entity.GetMovement();
		if (movement && !movement->IsFinished())
		{
			LuaContext::PushMovement(l, *movement);
			return 1;
		}
		return 0;
	});
}

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

int LuaContext::movement_path_finding_api_create(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		int speed = LuaTools::CheckInt(l, 1);
		bool ingore = LuaTools::OptBoolean(l, 2, false);
		std::shared_ptr<PathFindingMovement> movement =
			std::make_shared<PathFindingMovement>(speed, ingore);

		PushMovement(l, *movement);
		return 1;
	});
}

int LuaContext::movement_path_finding_api_set_target(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& pathFindingMovement = *std::static_pointer_cast<PathFindingMovement>
			(CheckUserdata(l, 1, module_path_movement_name));

		if (lua_isuserdata(l, 2))
		{
			auto& entity = CheckEntity(l, 2);
			pathFindingMovement.SetTarget(entity);
		}
		else if (LuaTools::IsPoint2(l, 2))
		{
			const auto& pos = LuaTools::CheckPoint2(l, 2);
			pathFindingMovement.SetTarget(pos);
		}
		else
		{
			LuaTools::ArgError(l, 2,
				"PathFindingMovement set the target which excepted point or entity.");
		}		
		return 0;
	});
}

