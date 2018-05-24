#pragma once

class Weapon;

class WeaponControlMode {
public:
	WeaponControlMode(Weapon& wepaon) :
		mWeapon(wepaon), mEnable(true) {}

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Leave() = 0;
	virtual bool IsFlip()const = 0;

	void SetEnable(bool enable) { mEnable = enable; }
	bool IsEnable()const { return mEnable; };
protected:
	bool mEnable;
	Weapon& mWeapon;
};

class RotateWeaponControl : public WeaponControlMode
{
public:
	RotateWeaponControl(Weapon& weapon);

	virtual void Enter();
	virtual void Update();
	virtual void Leave();
	virtual bool IsFlip()const;

private:
	bool mIsFliped;
};