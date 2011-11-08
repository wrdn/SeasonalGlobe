#include "SeasonalWindow.h"
#include "OBJFile.h"
#include "float4.h"
#include "Mat44.h"

#include <WinUser.h>
#include <iostream>
using namespace std;

SeasonalWindow::SeasonalWindow() : clearColor(Color::BLACK)
{
	windowRes[0] = DEFAULT_WIDTH;
	windowRes[1] = DEFAULT_HEIGHT;
};

const Color4f& SeasonalWindow::GetClearColor() const
{
	return clearColor;
};

void SeasonalWindow::SetClearColor(const Color4f &c)
{
	clearColor = c;
};

const u32* const SeasonalWindow::GetWindowRes() const
{
	return windowRes;
};
void SeasonalWindow::SetWindowResolution(const u32 width, const u32 height)
{
	windowRes[0] = width;
	windowRes[1] = height;
	SetSize(windowRes[0], windowRes[1]);

	glViewport(0,0,800,600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)windowRes[0] / (double)windowRes[1],
		1,50.0);
	glMatrixMode(GL_MODELVIEW);
};

void SeasonalWindow::SwitchFullscreen()
{
	SetFullscreen(!GetFullscreen());
};

const bool SeasonalWindow::IsFullScreen() const
{
	return GetFullscreen();
};

void SeasonalWindow::OnDisplay()
{
	gameTime.Update();

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

	scn.Draw(gameTime);
	
	SwapBuffers();
};

void SeasonalWindow::OnIdle()
{
	Redraw();
};

void SeasonalWindow::OnKeyboard(i32 key, bool down)
{
	if(key == VK_ESCAPE && down)
	{
		Close();
	}
};

void SeasonalWindow::OnCreate()
{
	GLWindowEx::OnCreate();
	SetWindowResolution(windowRes[0], windowRes[1]);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	scn.Load();

	gameTime.Init();

	//glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
};

void SeasonalWindow::OnDestroy()
{
};