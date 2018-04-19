#include "game\weaponControl.h"
#include "game\weapon.h"
#include "entity\player.h"
#include "utils\geometry.h"

RotateWeaponControl::RotateWeaponControl(Weapon & weapon) :
	WeaponControlMode(weapon)
{
}

void RotateWeaponControl::Enter()
{
}

void RotateWeaponControl::Update()
{
	auto weaponSprite = mWeapon.GetWeaponSprite();
	if (weaponSprite)
	{
		auto entity = mWeapon.GetEntity();
		if (entity == nullptr)
			return;

		float degree = Geometry::Degree(entity->GetFacingDegree());
		degree = degree >= 180 ? degree - 360 : degree;
		bool fliped = fabsf(degree) > 90;

		weaponSprite->SetFlipY(fliped);
		weaponSprite->SetRotated(-degree);
	}
}

void RotateWeaponControl::Leave()
{
}
