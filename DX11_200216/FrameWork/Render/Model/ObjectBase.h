#pragma once
#include "FrameWork/Game/Transform/Transform.h"

class BoxCollision;

class ObjectBase
{
protected:
	Transform* transform;
	bool IgnoreShadow;
	bool isShadow;

	DWORD CullMode;
	BoxCollision* collision;

	bool isActive;

public:
	ObjectBase();
	virtual ~ObjectBase();
	void Update(float timeDelta);
	void SetTransform(Transform* transform);
	virtual Transform* GetTransform() { return transform; }
	void Render() {
		if (this->isActive)
		{
			transform->SetVS();

			this->BaseObjectRender();
		}
	}

	void RenderShadow() {
		if (this->isActive)
			this->BaseObjectRenderShadow();
	}

	void SetActive(bool bActive);

	bool IsActive() {
		return this->isActive;
	}

	void ComputeBoundBox();
protected:
	virtual void BaseObjectEnable() = 0;
	virtual void BaseObjectDisable() = 0;
	virtual void BaseObjectUpdate(float timeDelta) = 0;
	virtual void BaseObjectNoActiveUpdate(float timeDelte) = 0;
	virtual void BaseObjectRender() = 0;
	virtual void BaseObjectPostRender() = 0;
	virtual void BaseObjectRenderShadow() = 0;
};