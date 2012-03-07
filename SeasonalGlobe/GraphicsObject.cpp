#include "GraphicsObject.h"


GraphicsObject::GraphicsObject(void)
	: position(), scale(1), mat(), xrotation(0), yrotation(0), zrotation(0), objectShader(0)
{
	ogl.Load();
}


GraphicsObject::~GraphicsObject(void)
{
}

void GraphicsObject::Draw()
{
	if(objectShader) { objectShader->Activate(); }

	for(std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		ogl.glActiveTexture((*it)->GetTextureSlot());
		(*it)->Activate();
	}
	
	mat.Activate(); // activate object material properties

	glPushMatrix();

	glTranslatef(position.x(), position.y(), position.z());

	glRotatef(xrotation, 1,0,0);
	glRotatef(yrotation, 0,1,0);
	glRotatef(zrotation, 0,0,1);

	glScalef(scale.x(), scale.y(), scale.z());

	gmodel.Draw();

	glPopMatrix();

	for(std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		(*it)->Deactivate();
	}
	if(objectShader) { objectShader->Deactivate(); }

	ogl.glActiveTexture(GL_TEXTURE0);
	ogl.glBindTextureEXT(GL_TEXTURE_2D, 0);
};

void GraphicsObject::DrawSimple()
{
	gmodel.Draw();
};