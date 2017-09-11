#include"lua\luaContext.h"
#include"game\savegame.h"
#include"game\game.h"
#include"core\fileData.h"
#include"game\app.h"

const string LuaContext::module_game_name = "Game";

void LuaContext::RegisterGameModule()
{
	static const luaL_Reg functions[] = {
		{"exists", game_api_exists},
		{"load", game_api_load },
		{nullptr,nullptr}
	};

	static const luaL_Reg methods[] = {
		{ "start", game_api_start},
		{ "save", game_api_save },
		{ "setValue", game_api_set_value},
		{ nullptr,nullptr }
	};

	static const luaL_Reg metamethos[] = {
		{ "__newindex", userdata_meta_newindex },
		{ "__index", userdata_meta_index},
		{ "__gc", userdata_meta_gc },
		{nullptr, nullptr}
	};
	RegisterType(module_game_name, functions, methods, metamethos);
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
	bool handle = false;
	PushGame(l, game.GetSavegame());
	handle = OnInput(event);
	lua_pop(l, 1);
	return handle;
}


/**
*	\brief 加载一个游戏存档
*/
int LuaContext::game_api_load(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const string& fileName = LuaTools::CheckString(l, 1);

		auto saveGame = std::make_shared<Savegame>(fileName);
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
			App& app = *savegame->GetLuaContext()->GetApp();
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


