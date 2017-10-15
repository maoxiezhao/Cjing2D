#include "game\equipment.h"
#include "game\savegame.h"

Equipment::Equipment(Savegame & savegame):
	mSavegame(savegame)
{
}

/**
*	\brief 获取当前生命值
*/
int Equipment::GetLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_LIFE);
}

/**
*	\brief 设置当前生命值
*/
void Equipment::SetLife(int life)
{
	mSavegame.SetInteger(Savegame::KEYWORD_CURRENT_LIFE, life);
}

/**
*	\brief 获取当前最大生命值
*/
int Equipment::GetMaxLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE);
}

/**
*	\brief 设置当前最大生命值
*/
void Equipment::SetMaxLife(int maxLife)
{
	mSavegame.SetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE, maxLife);
}
