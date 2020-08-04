#include "Framework.h"
#include "ObjectBase.h"

ObjectBase::ObjectBase()
{
	transform = new Transform();
}

ObjectBase::~ObjectBase()
{
	delete transform;
}

void ObjectBase::Update(float timeDelta)
{
	transform->UpdateWorld();

	BaseObjectUpdate(timeDelta);
}

void ObjectBase::SetTransform(Transform* transform)
{
	if (this->transform)
		delete this->transform;

	this->transform = transform;
}

void ObjectBase::SetActive(bool bActive)
{
	this->isActive = bActive;
}

void ObjectBase::ComputeBoundBox()
{
}
