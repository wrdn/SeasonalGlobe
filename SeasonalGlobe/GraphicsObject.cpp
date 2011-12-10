#include "GraphicsObject.h"


GraphicsObject::GraphicsObject(void)
	: position(), scale(1), mat(), xrotation(0), yrotation(0), zrotation(0), tex(0), objectShader(0)
{
}


GraphicsObject::~GraphicsObject(void)
{
}

void GraphicsObject::Draw()
{
	if(tex)
		tex->Activate();
	if(objectShader)
		objectShader->Activate();

	/*glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient.GetVec());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse.GetVec());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular.GetVec());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);*/
	mat.Activate(); // activate object material properties

	glPushMatrix();

	glTranslatef(position.x(), position.y(), position.z());

	glRotatef(xrotation, 1,0,0);
	glRotatef(yrotation, 0,1,0);
	glRotatef(zrotation, 0,0,1);

	glScalef(scale.x(), scale.y(), scale.z());

	gmodel.Draw();

	glPopMatrix();

	if(objectShader)
		objectShader->Deactivate();
	if(tex)
		tex->Deactivate();
};

void GraphicsObject::DrawSimple()
{
	gmodel.Draw();
};