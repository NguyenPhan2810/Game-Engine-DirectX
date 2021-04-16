#pragma once

#include <Windows.h>
#include <xnamath.h>

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 direction;
	float pad; // Pad the last float so we can set an array of lights if we wanted.
}; 

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 position;
	float range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 att; //Attenuation
	float pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct Material
{
	// Set all values to 0	
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular; // w = SpecPower
	//XMFLOAT4 reflect; // Ignore for now
};