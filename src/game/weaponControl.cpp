#include "game\weaponControl.h"
#include "game\weapon.h"
#include "entity\player.h"
#include "utils\geometry.h"

RotateWeaponControl::RotateWeaponControl(Weapon & weapon) :
	WeaponControlMode(weapon),
	mIsFliped(false)
{
}

void RotateWeaponControl::Enter()
{
}

void RotateWeaponControl::Update()
{
	if (!IsEnable())
		return;

	auto weaponSprite = mWeapon.GetWeaponSprite();
	if (weaponSprite)
	{
		auto entity = mWeapon.GetEntity();
		if (entity == nullptr)
			return;

		float degree = (entity->GetFacingDegree());
		degree = degree >= 180 ? degree - 360 : degree;
		mIsFliped = fabsf(degree) > 90;

		weaponSprite->SetFlipY(mIsFliped);
		auto& weaponInstance = mWeapon.GetWeaponInstance();
		weaponInstance.SetFacingDegree(-degree);
	}
}

void RotateWeaponControl::Leave()
{
}

bool RotateWeaponControl::IsFlip() const
{
	return mIsFliped;
}
