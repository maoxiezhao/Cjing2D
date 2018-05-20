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
GAME_PROP_MAX_SP = "game_prop_max_sp"

---------------- Common event define ------------------------
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

EVENT_GAME_START_DIALOG = 11
EVENT_GAME_START_QUESTIONS = 12

EVENT_ITEM_COUNT_CHANGE = 50
EVENT_WEAPON_SWAP_BULLET = 51
EVENT_WEAPON_FIRE_BULLET = 52

EVENT_ENTITY_EQUIP_WEAPON    = 500
EVENT_ENTITY_UNEQUIP_WEAPON  = 501
EVENT_ENTITY_PROPERTY_CHANGE = 1000

---------------- Entity Type ----------------------------------
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

-- Entity perperty
ENTITY_PROPERTY_HP = 1
ENTITY_PROPERTY_MAX_HP = 2
ENTITY_PROPERTY_SP = 3
ENTITY_PROPERTY_MAX_SP = 4 
ENTITY_PROPERTY_DEMAGE = 5

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
	"utf8",
}

-- text align
FONT_ALIGN_LEFT		= 1 << 0	--// 默认文本左对齐
FONT_ALIGN_CENTER   = 1 << 1	--// 文本居中对齐
FONT_ALIGN_RIGHT    = 1 << 2	--// 文本右对齐
-- Vertical align
FONT_ALIGN_TOP      = 1 << 3	--// 文本水平顶部对齐
FONT_ALIGN_MIDDLE   = 1 << 4	--// 文本水平居中对齐
FONT_ALIGN_BOTTOM   = 1 << 5	--// 文本水平底部对齐
FONT_ALIGN_BASELINE = 1 << 6    --//默认，对齐文本于baseline