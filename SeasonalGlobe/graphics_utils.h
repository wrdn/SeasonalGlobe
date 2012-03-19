#pragma once

#include "GraphicsObject.h"
#include "World.h"
#include "ResourceManager.h"

// This file contains a set of utility functions to create and draw specific graphics objects
// NOTE: drawing functions below all use immediate mode

// Create imposter mesh (2 quads perpendicular to each other)
MeshHandle CreateImposterModel();

// Create billboard mesh (1 quad)
MeshHandle CreateBillboardModel();

// draw cube
void cube();

// draw a textured cube
void texcube(Texture &t);

// draw a flat plane
void floor();

// draw flat plane at specific position and scale. Used for water
void DrawFloor(const float3 &floorPos, const float3 &floorScale);