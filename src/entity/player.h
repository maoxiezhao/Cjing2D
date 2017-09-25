#ifndef _HERO_H_
#define _HERO_H_

#include"common\common.h"
#include"entity\entity.h"
#include"game\animationSprite.h"
#include"game\gameCommands.h"

class PlayerSprite;

/**
*	\brief 可操纵的玩家类基类
*/
class Player : public Entity
{
public:
	Player();

	/** system operator */
	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	/** Notify command event */
	virtual void NotifyMovementChanged();

	/** status manager */
	void PlaceOnMap(Map& map);

	/** getter/setter */
	int GetWalkingSpeed()const;
	PlayerSprite& GetPlayerSprites();
	virtual EntityType GetEntityType()const;
	void SetBindDirectoinByGameCommand(bool binded);

private:
	std::unique_ptr<PlayerSprite> mPlayerSprites;	// 当前的sprite合集

	Point2 mDisplayedPos;		// sprite 显示的位置
	bool mIsBindDirectionByGameCommand;

	int mNormalWalkingSpeed;
	int mCurWalkingSpeed;
};

#endif