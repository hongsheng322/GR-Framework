#include "EntityBase.h"

EntityBase::EntityBase() 
	: position(0.0f, 0.0f, 0.0f)
	, scale(1.0f, 1.0f, 1.0f)
	, isDone(false)
	, isFixed(false)
	, m_bCollider(false)
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::Update(double _dt)
{
}

void EntityBase::Render()
{
}

void EntityBase::RenderUI()
{
}

bool EntityBase::IsDone()
{
	return isDone;
}

void EntityBase::SetIsDone(bool _value)
{
	isDone = _value;
}

bool EntityBase::IsFixed()
{
	return isFixed;
}

void EntityBase::SetIsFixed(const bool _value)
{
	isFixed = _value;
}

// Check if this entity has a collider class parent
bool EntityBase::HasCollider(void) const
{
	return m_bCollider;
}

// Set the flag to indicate if this entity has a collider class parent
void EntityBase::SetCollider(const bool _value)
{
	m_bCollider = _value;
}

bool EntityBase::HasPhysic(void) const
{
	return m_bPhysic;
}

void EntityBase::SetPhysic(const bool _value)
{
	m_bPhysic = _value;
}
