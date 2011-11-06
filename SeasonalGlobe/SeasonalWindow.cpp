#include "SeasonalWindow.h"
#include "OBJFile.h"
#include <WinUser.h>
#include "float4.h"

const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;

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

const int* const SeasonalWindow::GetWindowRes() const
{
	return (const int* const)windowRes;
};
void SeasonalWindow::SetWindowResolution(const int width, const int height)
{
	windowRes[0] = width;
	windowRes[1] = height;
	SetSize(windowRes[0], windowRes[1]);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	
	SwapBuffers();
};

void SeasonalWindow::OnIdle()
{
	Redraw();
};

void SeasonalWindow::OnKeyboard(int key, bool down)
{
	if(key == VK_ESCAPE && down)
	{
		Close();
	}
};

//void CalculateTriNormals(f32* v1, f32 *v2, f32 *v3)
//{
//	f32 n1[3], n2[3], n3[3];
//
//	{
//		f32 t[3], v[3];
//		fmin3(v2,v1,t);
//		fmin3(v3,v1,v);
//		fcross3(t,v,n1);
//	}
//
//	{
//		f32 t[3], v[3];
//		fmin3(v3,v2,t);
//		fmin3(v1,v2,v);
//		fcross3(t,v,n2);
//	}
//
//	{
//		f32 t[3], v[3];
//		fmin3(v1,v3,t);
//		fmin3(v2,v3,v);
//		fcross3(t,v,n3);
//	}
//	printf("");
//}

void SeasonalWindow::OnCreate()
{
	OBJFile f;
	f.ParseOBJFile("Data/cube.obj");
	f.GetModels()[0]->RecalculatePerVertexNormals();

	GLWindowEx::OnCreate();
	SetWindowResolution(windowRes[0], windowRes[1]);
};

void SeasonalWindow::OnDestroy()
{
};