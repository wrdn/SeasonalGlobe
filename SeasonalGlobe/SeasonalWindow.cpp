#include "SeasonalWindow.h"
#include "OBJFile.h"
#include "float4.h"
#include "Mat44.h"

#include <WinUser.h>
#include <iostream>
using namespace std;

const c8* SeasonalWindow::DEFAULT_WINDOW_TITLE = "Seasonal Globe";

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

	ResetPerspective();
};

void SeasonalWindow::ResetPerspective() const
{
	glViewport(0,0,windowRes[0],windowRes[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)windowRes[0] / (double)windowRes[1],0.3f,200.0);
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
	else if(key == VK_RETURN && !down)
	{
		if(glIsEnabled(GL_CULL_FACE))
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
		}
	}
	else if(key == VK_LEFT && !down)
	{
		int params[2];
		glGetIntegerv(GL_POLYGON_MODE, params);

		if(params[0] == GL_LINE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else if(params[0] == GL_POINT)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
};

void SeasonalWindow::OnCreate()
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	scn.Load();

	gameTime.Init();

	GLWindowEx::OnCreate();
	SetTitle(DEFAULT_WINDOW_TITLE);
	SetWindowResolution(windowRes[0], windowRes[1]);

	//glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
};

void SeasonalWindow::OnDestroy()
{
};