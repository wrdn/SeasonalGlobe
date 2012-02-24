#pragma once

#include "GraphicsObject.h"
#include "World.h"

Model* CreateImposterModel();

Model* CreateBillboardModel();

void cube();

void texcube(const Texture &t);

void floor();

void DrawFloor(const float3 &floorPos, const float3 &floorScale);