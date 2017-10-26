#ifndef _HERO_H_
#define _HERO_H_

#include"common\common.h"
#include"entity\entity.h"
#include"game\animationSprite.h"
#include"game\gameCommands.h"

class PlayerSprite;
class Equipment;

/**
*	\brief 可操纵的玩家类基类
*/
class Player : public Entity
{
public:
	Player(Equipment& equipment);

	/** system operator */
	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	/** Notify command event */
	virtual void NotifyMovementChanged();
	virtual void NotifyPositonChanged();

	/** status manager */
	void PlaceOnMap(Map& map);
	void CheckPosition();

	/** getter/setter */
	int GetWalkingSpeed()const;
	PlayerSprite& GetPlayerSprites();
	virtual EntityType GetEntityType()const;
	void SetBindDirectoinByGameCommand(bool binded);

private:
	std::unique_ptr<PlayerSprite> mPlayerSprites;	// 当前的sprite合集
	Equipment& mEquipment;		// 当前player数值管理者，包括装备管理

	Point2 mDisplayedPos;		// sprite 显示的位置
	bool mIsBindDirectionByGameCommand;

	int mNormalWalkingSpeed;
	int mCurWalkingSpeed;
};

#endif