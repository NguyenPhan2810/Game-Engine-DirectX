#pragma once

#include "d3dUtil.h"
#include "BaseComponent.h"

class Transform : public BaseComponent
{
public:
	Transform();
	~Transform() override;

	virtual void Init() override;
	virtual void Update(float dt = 0) override;
	virtual void Draw() override;

private:
	Transform(const Transform& renderer) {}
	void operator=(const Transform& renderer) {}

public:
	void Translate(const XMFLOAT3& displacement);
	void Rorate(const XMFLOAT3& rotationOrigin, float radian);
	void Scale(const XMFLOAT3& scaleElements);

	XMMATRIX LocalToWorldMatrix() const;

protected:
	XMMATRIX mWorldMatrix;
};
