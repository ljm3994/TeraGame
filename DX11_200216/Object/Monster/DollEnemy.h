#pragma once
#include "Enemy.h"
class DollEnemy : public Enemy
{
private:

public:
	DollEnemy();
	~DollEnemy();
	// Enemy��(��) ���� ��ӵ�
	virtual void BaseObjectUpdate(float timeDelta) override;

	virtual void BaseObjectNoActiveUpdate(float timeDelte) override;

	virtual void BaseObjectRender() override;

	virtual void BaseObjectRenderShadow() override;

};