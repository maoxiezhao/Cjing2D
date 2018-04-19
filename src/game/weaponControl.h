#pragma once

class Weapon;

class WeaponControlMode {
public:
	WeaponControlMode(Weapon& wepaon) :
		mWeapon(wepaon) {}

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Leave() = 0;
protected:
	Weapon& mWeapon;
};

class RotateWeaponControl : public WeaponControlMode
{
public:
	RotateWeaponControl(Weapon& weapon);

	virtual void Enter();
	virtual void Update();
	virtual void Leave();
};