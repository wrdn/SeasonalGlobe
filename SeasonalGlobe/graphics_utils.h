#pragma once

#include "GraphicsObject.h"
#include "World.h"
#include "ResourceManager.h"

MeshHandle CreateImposterModel();

MeshHandle CreateBillboardModel();

void cube();

void texcube(Texture &t);

void floor();

void DrawFloor(const float3 &floorPos, const float3 &floorScale);