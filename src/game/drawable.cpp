#include "drawable.h"

Drawable::Drawable():
	mPos(0,0),
	mRotate(0.0f),
	mGlobalZOrder(0),
	mSuspended(false)
{
}

Drawable::~Drawable()
{
	mMovements.clear();
}

/**
*	\brief 更新drawable中的Movements
*/
void Drawable::Update()
{
	if (IsSuspended() )
	{
		return;
	}
	for (auto& movment : mMovements)
	{
		// do movement
	}
}

Point2 Drawable::GetPos() const
{
	return mPos;
}

void Drawable::SetPos(const Point2 & pos)
{
	mPos = pos;
}

float Drawable::GetRotated() const
{
	return mRotate;
}

void Drawable::SetRotated(float angle)
{
	mRotate = angle;
}

float Drawable::GetGlobalOrder() const
{
	return mGlobalZOrder;
}

void Drawable::SetGlobalOrder(float order)
{
	mGlobalZOrder = order;	
}

void Drawable::SetSuspended(bool suspended)
{
	mSuspended = suspended;
}

bool Drawable::IsSuspended() const
{
	return mSuspended;
}

