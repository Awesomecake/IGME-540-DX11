#pragma once
#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light {
	Light() : Type(LIGHT_TYPE_DIRECTIONAL), Direction(1,0,0), Range(10), Position(0,0,0), Intensity(1), Color(1, 1, 1) { }

	int Type;
	DirectX::XMFLOAT3 Direction;
	float Range;
	DirectX::XMFLOAT3 Position;
	float Intensity;
	DirectX::XMFLOAT3 Color;
	float SpotFalloff;
	DirectX::XMFLOAT3 Padding;
};