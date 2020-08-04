#pragma once
#include <FrameWork\Render\Model\ObjectBase.h>
#include "FrameWork\States\RasterizerState.h"
#include "FrameWork\States\DepthStencilState.h"
#include "FrameWork\States\BlendState.h"
#include "FrameWork\Buffer\Model\TrailBoneBuffer.h"
#include "FrameWork\Buffer\Model\TrailColorBuffer.h"

class Trail : public ObjectBase
{
private:
	TrailBoneBuffer* bonebuffer;
	TrailColorBuffer* colorbuffer;
	VertexBuffer* vertexbuffer;
	Shader* shader;
	ModelInstance* target;
	Texture* texture;
	RasterizeState* CullMode[2];
	DepthStencilState* DepthMode[2];
	BlendState* BlendMode[2];

	string boneName;
	D3DXMATRIX matrix;

	int Count;
	bool isBone;
	bool isCull;
	bool isDepth;

	bool isFirst;

	float ChangedTime;
	float DeltaTime;
public:
	Trail(int Count = 128);
	~Trail();

	void SetMatrix();
	void CreateData();
	void SetIsCull(bool val) { isCull = val; }
	void SetIsDepth(bool val) { isDepth = val; }
	// ObjectBase을(를) 통해 상속됨
	virtual void BaseObjectEnable() override;
	virtual void BaseObjectDisable() override;
	virtual void BaseObjectUpdate(float timeDelta) override;
	virtual void BaseObjectNoActiveUpdate(float timeDelte) override;
	virtual void BaseObjectRender() override;
	virtual void BaseObjectRenderShadow() override;

	void SetTarget(string ModelKey, string BoneName = "");

	// ObjectBase을(를) 통해 상속됨
	virtual void BaseObjectPostRender() override;
};