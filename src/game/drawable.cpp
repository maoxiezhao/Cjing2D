#include "drawable.h"
#include "lua\luaContext.h"
#include "movements\movement.h"

Drawable::Drawable():
	mPos(0,0),
	mRotate(0.0f),
	mGlobalZOrder(0),
	mSuspended(false),
	mMovement(nullptr)
{
}

Drawable::~Drawable()
{
	mMovements.clear();
	StopMovement();
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
	if (mMovement != nullptr)
	{
		mMovement->Update();
		// 如果移动完毕，则移除movement
		if (mMovement != nullptr && mMovement->IsFinished() )
		{
			StopMovement();
		}
	}

	// 处理所有的movement，暂时不用
	for (auto& movement : mMovements)
	{
		movement->Update();
		// 如果移动完毕，则移除movement
		if (movement != nullptr && movement->IsFinished())
		{
			// mMovementToRemoves.pushback(movment);
		}
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

void Drawable::StopMovement()
{
	mMovement = nullptr;
}

/**
*	\brief 添加一个新的movment组件
*/
void Drawable::StartMovement(const std::shared_ptr<Movement>& movement)
{
	StopMovement();
	mMovement = movement;
	mMovement->SetDrawable(this);
	mMovement->SetSuspended(IsSuspended());
}

const std::shared_ptr<Movement>& Drawable::GetMovement()
{
	return mMovement;
}

const string Drawable::GetLuaObjectName() const
{
	return LuaContext::module_drawable_name;;
}

