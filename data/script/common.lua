-- 全局的定义 --
CLIENT_LUA_DEBUG = true


-- 系统级函数定义，必须和C++端对应，在C++的const实现后剔除
CLIENT_LUA_MAIN_START = 1
CLIENT_LUA_MAIN_UPDATE = 2
CLIENT_LUA_MAIN_STOP = 3
CLIENT_LUA_MAIN_RENDER = 4
CLIENT_LUA_FIRE_EVENT = 5
CLIENT_LUA_INPUT_KEY_DOWN = 6
CLIENT_LUA_INPUT_KEY_UP = 7

-- Game Define
GAME_DIRECTION_NONE = -1
GAME_DIRECTION_RIGHT = 0
GAME_DIRECTION_UP = 1
GAME_DIRECTION_LEFT = 2 
GAME_DIRECTION_DOWN = 3

-- Game save value define
GAME_PROP_IS_NEW_GAME = "game_prop_is_new_game"

-- Common event define
EVENT_GAME_START = 1
EVENT_GAME_END = 2
EVENT_GAME_MAP_ENTER = 3
EVENT_GAME_MAP_LEAVE = 4
EVENT_GAME_PLAYR_ENTER = 5
EVENT_GAME_PLAYR_LEAVE = 6
EVENT_GAME_ENTITY_ENTER = 7
EVENT_GAME_ENTITY_LEAVE = 8

EVENT_INPUT_KEY_DOWN = 9
EVENT_INPUT_KEY_UP = 10

-- Entity Type
ENTITY_TYPE_TITLE          = 1   --/** 地图块 */
ENTITY_TYPE_DYNAMIC_TITLE  = 2   --/** 动态地图块 */
ENTITY_TYPE_DESTIMATION    = 3	 --/** 目标点 */
ENTITY_TYPE_CAMERA		   = 4	 --/** 相机对象 */
ENTITY_TYPE_PLAYRE		   = 5   --/** 玩家对象 */
ENTITY_TYPE_PICKABLE	   = 6   --/** 可拾取对象 */
ENTITY_TYPE_ENEMEY		   = 7   --/** 敌人 */
ENTITY_TYPE_BULLET	   	   = 8   --/** 子弹 */
ENTITY_TYPE_UNKNOW		   = 9   --/** 未知的类型 */

-- Entity Attack
ATTACK_COMBAT 	 = 1		--/** 近战 */
ATTACK_BULLET 	 = 2		--/** 子弹伤害 */
ATTACK_BOOMERANG = 3		--/** 爆炸伤害 */

REACTION_IGNORE  = 1   		 --/** 忽略攻击 */			
REACTION_HURT	 = 2   		 --/** 受伤 */
REACTION_CUSTOM  = 3  		 --/** 在脚本中处理 */

-- Origin env function
ORGIN_ENV_LIST = {
	"assert", 
	"collectgarbage", 
	"dofile", 
	"error", 
	"getmetatable", 
	"ipairs", 
	"load", 
	"loadfile", 
	"next", 
	"pairs", 
	"pcall", 
	"print", 
	"rawequal", 
	"rawget", 
	"rawlen", 
	"rawset", 
	"require", 
	"select", 
	"setmetatable", 
	"tonumber", 
	"tostring", 
	"type", 
	"xpcall", 
	"debug", 
	"io",
	"math", 
	"string", 
	"table", 
}