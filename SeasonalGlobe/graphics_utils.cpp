#include "graphics_utils.h"

Model* CreateImposterModel()
{
	Model *imposterModel = new Model();
	VERTEX *imposterVerts = new VERTEX[8];
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
	
	imposterModel->SetVertexArray(imposterVerts, 8);
	
	u32 *imposterIndices = new u32[16];
	imposterIndices[0] = 1; imposterIndices[1] = 0;
	imposterIndices[2] = 3; imposterIndices[3] = 1;
	imposterIndices[4] = 3; imposterIndices[5] = 2;
	imposterIndices[6] = 5; imposterIndices[7] = 4;
	imposterIndices[8] = 7; imposterIndices[9] = 5;
	imposterIndices[10] = 7; imposterIndices[11] = 6;

	imposterModel->SetIndicesArray(imposterIndices, 16);

	imposterModel->BuildVBO();
	return imposterModel;
};

Model* CreateBillboardModel()
{
	// Billboard model (textured quad)
	Model *billboardModel = new Model();
	VERTEX *billboardVertices = new VERTEX[4];
	billboardVertices[0] = VERTEX(float3(-0.5f,1,0), float3(0,0,1), float2(0,1));
	billboardVertices[1] = VERTEX(float3(0.5,1,0),   float3(0,0,1), float2(1,1));
	billboardVertices[2] = VERTEX(float3(-0.5f,0,0), float3(0,0,1), float2(0,0));
	billboardVertices[3] = VERTEX(float3(0.5f,0,0),  float3(0,0,1), float2(1,0));
	billboardModel->SetVertexArray(billboardVertices, 4);

	u32 *billboardIndices = new u32[6];
	billboardIndices[0] = 0;
	billboardIndices[1] = 2;
	billboardIndices[2] = 3;
	billboardIndices[3] = 0;
	billboardIndices[4] = 3;
	billboardIndices[5] = 1;
	billboardModel->SetIndicesArray(billboardIndices, 6);
	billboardModel->BuildVBO();
	return billboardModel;
};

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

void texcube(Texture &t)
{
	glEnable(GL_TEXTURE_2D);
	t.Activate();
	cube();
	t.Deactivate();
	glDisable(GL_TEXTURE_2D);
}

void floor()
{
	glPushMatrix();
	//glScalef(0.5,0.5,0.5);

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

void DrawFloor(const float3 &floorPos, const float3 &floorScale)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(floorPos.x(), floorPos.y(), floorPos.z());
	glScalef(floorScale.x(), floorScale.y(), floorScale.z());
	floor(); // draw floor into stencil
	glEnable(GL_TEXTURE_2D);
};