#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"game\savegame.h"
#include"game\game.h"
#include"core\fileData.h"
#include"game\app.h"

const string LuaContext::module_game_name = "Game";

void LuaContext::RegisterGameModule()
{
	LuaBindClass<Game> gameClass(l, module_game_name);
	gameClass.AddDefaultMetaFunction();

	gameClass.AddFunction("Exists", game_api_exists);
	gameClass.AddFunction("Load", game_api_load);

	gameClass.AddMethod("Start", game_api_start);
	gameClass.AddMethod("Save", game_api_save);
	gameClass.AddMethod("SetValue", game_api_set_value);
	gameClass.AddMethod("GetValue", game_api_get_value);
	gameClass.AddMethod("SetLife", game_api_set_life);
	gameClass.AddMethod("GetLife", game_api_get_life);
	gameClass.AddMethod("GetMaxLife", game_api_get_max_life);
	gameClass.AddMethod("SetMaxLife", game_api_set_max_life);
	gameClass.AddMethod("SetStartLocation", game_api_set_start_location);
	gameClass.AddMethod("GetPlayer", game_api_get_player);
}

void LuaContext::PushGame(lua_State*l, Savegame& saveGame)
{
	PushUserdata(l, saveGame);
}

std::shared_ptr<Savegame> LuaContext::CheckSavegame(lua_State*l, int index)
{
	return std::dynamic_pointer_cast<Savegame>(CheckUserdata(l, index, module_game_name));
}

bool LuaContext::IsSavegame(lua_State*l, int index)
{
	return IsUserdata(l, index, module_game_name);
}

void LuaContext::OnGameStart(Game& game)
{
	PushGame(l ,game.GetSavegame());
	OnStart();
	lua_pop(l, 1);
}

void LuaContext::OnGameUpdate(Game & game)
{
	PushGame(l, game.GetSavegame());
	OnUpdate();
	lua_pop(l, 1);
}

void LuaContext::OnGameFinish(Game & game)
{
	PushGame(l, game.GetSavegame());
	OnFinish();
	lua_pop(l, 1);
}

void LuaContext::OnGameDraw(Game & game)
{
	PushGame(l, game.GetSavegame());
	OnDraw();
	lua_pop(l, 1);
}

bool LuaContext::OnGameInput(Game & game, const InputEvent & event)
{
	//bool handle = false;
	//PushGame(l, game.GetSavegame());
	//handle = OnInput(event);
	//lua_pop(l, 1);
	//return handle;

	return false;
}


/**
*	\brief 加载一个游戏存档
*/
int LuaContext::game_api_load(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string& fileName = LuaTools::CheckString(l, 1);

		auto saveGame = std::make_shared<Savegame>(GetLuaContext(l).GetApp(), fileName);
		saveGame->Init();

		PushGame(l, *saveGame);
		return 1;
	});
}

/**
*	\brief 是否存在一个游戏存档
*/
int LuaContext::game_api_exists(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string& fileName = LuaTools::CheckString(l, 1);

		bool exists = FileData::IsFileExists(fileName);
		lua_pushboolean(l, exists);
		return 1;
	});
}

/**
*	\brief 保存一个游戏存档
*/
int LuaContext::game_api_save(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		savegame.SaveGameToLocal();

		return 0;
	});
}

/**
*	\brief 开始游戏
*/
int LuaContext::game_api_start(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		std::shared_ptr<Savegame> savegame = CheckSavegame(l, 1);

		Game* game = savegame->GetGame();
		if (game != nullptr)
		{	// 已经存在游戏则直接加载
			game->Start();
		}
		else
		{	// 新的游戏，则创建一个新的Game
			App& app = savegame->GetLuaContext().GetApp();
			Game* game = new Game(&app, savegame);
			app.SetGame(game);
		}
		return 0;
	});
}

/**
*	\brief 实现game:saveValue(key, value)
*/
int LuaContext::game_api_set_value(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		const std::string& key = LuaTools::CheckString(l, 2);

		switch (lua_type(l, 3))
		{
		case LUA_TBOOLEAN:
			savegame.SetBoolean(key, LuaTools::CheckBoolean(l, 3));
			break;
		case LUA_TNUMBER:
			savegame.SetInteger(key, LuaTools::CheckInt(l, 3));
			break;
		case LUA_TSTRING:
			savegame.SetString(key, LuaTools::CheckString(l, 3));
			break;
		case LUA_TNIL:
			savegame.UnSet(key);
			break;
		default:
			LuaTools::Error(l, "Invalid value.");
			break;
		}
		return 0;
	});
}

/**
*	\brief 实现game:GetValue(key)
*/
int LuaContext::game_api_get_value(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		const std::string& key = LuaTools::CheckString(l, 2);

		auto valueType = savegame.GetValueType(key);
		if (valueType == Savegame::VALUE_NONE)
			return 0;

		if (valueType == Savegame::VALUE_STRING) 
		{
			const std::string& svalue = savegame.GetString(key);
			lua_pushstring(l, svalue.c_str());
		}
		else if (valueType == Savegame::VALUE_INTEGER)
		{
			int ivalue = savegame.GetInteger(key);
			lua_pushinteger(l, ivalue);
		}
		else if(valueType == Savegame::VALUE_BOOLEAN)
		{
			bool bvalue = savegame.GetBoolean(key);
			lua_pushboolean(l, bvalue);
		}
		return 1;
	});
}

/**
*	\brief 实现game:setStartLocation(mapID)
*
*	设置game的开始地图和位置
*/
int LuaContext::game_api_set_start_location(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		const std::string mapID = LuaTools::CheckString(l, 2);
		savegame.SetString(Savegame::KEYWORD_START_MAP, mapID);

		return 0;
	});
}

/**
*	\brief 实现game:getLife()
*/
int LuaContext::game_api_get_life(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		int life = savegame.GetEquipment().GetLife();
		lua_pushinteger(l, life);
		return 1;
	});
}

/**
*	\brief 实现game:getMaxLife()
*/
int LuaContext::game_api_get_max_life(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		int life = savegame.GetEquipment().GetMaxLife();
		lua_pushinteger(l, life);
		return 1;
	});
}

/**
*	\brief 实现game:setLife(life)
*/
int LuaContext::game_api_set_life(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		int life = LuaTools::CheckInt(l, 2);
		savegame.SetInteger(Savegame::KEYWORD_CURRENT_LIFE, life);

		return 0;
	});
}

/**
*	\brief 实现game:setMaxLife(maxLife)
*/
int LuaContext::game_api_set_max_life(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		int maxLife = LuaTools::CheckInt(l, 2);
		savegame.SetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE, maxLife);

		return 0;
	});
}

/**
*	\brief 实现game:GetPlayer()
*/
int LuaContext::game_api_get_player(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		Game* game = savegame.GetGame();
		if (game != nullptr)
		{
			auto player = game->GetPlayer();
			if (player != nullptr)
			{
				PushUserdata(l, *player);
				return 1;
			}
		}
		return 0;
	});
}


