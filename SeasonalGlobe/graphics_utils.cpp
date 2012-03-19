#include "graphics_utils.h"

// create imposter (2 quads) and return mesh handle
MeshHandle CreateImposterModel()
{
	MeshHandle mh = CreateMesh("imposterMesh");

	VERTEX imposterVerts[8];
	const f32 moduvtop = 0.95f;

	// first quad
	imposterVerts[0] = VERTEX(float3(-1,1,0),  float3(0,0,1), float2(0,moduvtop));
	imposterVerts[1] = VERTEX(float3(1,1,0),   float3(0,0,1), float2(1,moduvtop));
	imposterVerts[2] = VERTEX(float3(1,-1,0),  float3(0,0,1), float2(1,0));
	imposterVerts[3] = VERTEX(float3(-1,-1,0), float3(0,0,1), float2(0,0));

	// second quad
	imposterVerts[4] = VERTEX(float3(0,1,-1),  float3(1,0,0), float2(0,moduvtop));
	imposterVerts[5] = VERTEX(float3(0,1,1),   float3(1,0,0), float2(1,moduvtop));
	imposterVerts[6] = VERTEX(float3(0,-1,1),  float3(1,0,0), float2(1,0));
	imposterVerts[7] = VERTEX(float3(0,-1,-1), float3(1,0,0), float2(0,0));
	
	u32 imposterIndices[16];
	imposterIndices[0] = 1; imposterIndices[1] = 0;
	imposterIndices[2] = 3; imposterIndices[3] = 1;
	imposterIndices[4] = 3; imposterIndices[5] = 2;
	imposterIndices[6] = 5; imposterIndices[7] = 4;
	imposterIndices[8] = 7; imposterIndices[9] = 5;
	imposterIndices[10] = 7; imposterIndices[11] = 6;

	// build vbo
	mh->BuildVBO(imposterVerts, 8, imposterIndices, 16);

	// return mesh handle
	return mh;
};

// create billboard (1 quad) and return mesh handle
MeshHandle CreateBillboardModel()
{
	// Billboard model (textured quad)
	MeshHandle mh = CreateMesh("billboardMesh");

	VERTEX billboardVertices[4];
	billboardVertices[0] = VERTEX(float3(-0.5f,1,0), float3(0,0,1), float2(0,1));
	billboardVertices[1] = VERTEX(float3(0.5,1,0),   float3(0,0,1), float2(1,1));
	billboardVertices[2] = VERTEX(float3(-0.5f,0,0), float3(0,0,1), float2(0,0));
	billboardVertices[3] = VERTEX(float3(0.5f,0,0),  float3(0,0,1), float2(1,0));
	
	u32 billboardIndices[6];
	billboardIndices[0] = 0;
	billboardIndices[1] = 2;
	billboardIndices[2] = 3;
	billboardIndices[3] = 0;
	billboardIndices[4] = 3;
	billboardIndices[5] = 1;

	// build vbo
	mh->BuildVBO(billboardVertices, 4, billboardIndices, 6);

	// return mesh handle
	return mh;
};

// draw cube (immediate mode)
void cube()
{
	// Front Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0,  1.0);
	glEnd();
	// Back Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0, -1.0);
	glEnd();
	// Top Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,1.0f,0.0f);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glEnd();
	// Bottom Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,-1.0f,0.0f);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glEnd();
	// Right face
	glBegin(GL_QUADS);
	glNormal3f(1.0f,0.0f,0.0f);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glEnd();
	// Left Face
	glBegin(GL_QUADS);
	glNormal3f(-1.0f,0.0f,0.0f);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glEnd();
};

// draw textured cube using Texture t (immediate mode)
void texcube(Texture &t)
{
	glEnable(GL_TEXTURE_2D);
	t.Activate();
	cube();
	t.Deactivate();
	glDisable(GL_TEXTURE_2D);
}

// draw floor plane
void floor()
{
	glPushMatrix();
	
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);

	glTexCoord2f(0,0);
	glVertex3f(-10.0f,0.0f,10.0f);

	glTexCoord2f(1,0);
	glVertex3f(10.0f,0.0f,10.0f);

	glTexCoord2f(1,1);
	glVertex3f(10.0f,0.0f,-10.0f);

	glTexCoord2f(0,1);
	glVertex3f(-10.0f,0.0f,-10.0f);
	glEnd();

	glPopMatrix();
}

// draw floor plane with translation and scale
void DrawFloor(const float3 &floorPos, const float3 &floorScale)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(floorPos.x(), floorPos.y(), floorPos.z());
	glScalef(floorScale.x(), floorScale.y(), floorScale.z());
	floor();
	glEnable(GL_TEXTURE_2D);
};