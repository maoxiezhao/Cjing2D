#include "drawable.h"
#include "lua\luaContext.h"
#include "movements\movement.h"

Drawable::Drawable():
	mPos(0,0),
	mRotate(0.0f),
	mGlobalZOrder(0),
	mLocalZOrder(0),
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
*	\brief ����drawable�е�Movements
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
		// ����ƶ���ϣ����Ƴ�movement
		if (mMovement != nullptr && mMovement->IsFinished() )
		{
			StopMovement();
		}
	}

	// �������е�movement����ʱ����
	for (auto& movement : mMovements)
	{
		movement->Update();
		// ����ƶ���ϣ����Ƴ�movement
		if (movement != nullptr && movement->IsFinished())
		{
			// mMovementToRemoves.pushback(movment);
		}
	}
}

void Drawable::Draw()
{
}

void Drawable::Draw(const Point2 & pos)
{
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
*	\brief ���һ���µ�movment���
*/
void Drawable::StartMovement(const std::shared_ptr<Movement>& movement)
{
	StopMovement();
	mMovement = movement;
	mMovement->SetDrawable(this);
	mMovement->SetSuspended(IsSuspended());
	mMovement->Start();
}

const std::shared_ptr<Movement>& Drawable::GetMovement()
{
	return mMovement;
}

const string Drawable::GetLuaObjectName() const
{
	return LuaContext::module_drawable_name;;
}

