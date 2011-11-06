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

void SeasonalWindow::OnKeyboard(i32 key, bool down)
{
	if(key == VK_ESCAPE && down)
	{
		Close();
	}
};

void SeasonalWindow::OnCreate()
{
	/*f32 matData[] = { 1,0,5,0, 0,2,0,3, 0,2,1,0, 7,0,0,4 };
	Mat44 ma(matData);
	Mat44 invma = ma.Inverse();
	f32 mdet = ma.Determinant();

	f32 matData[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
	Mat44 ma(matData);
	Mat44 mb, r8 = ma.Transpose();
	Mat44 r1 = ma.Add(mb), r2 = ma.Add(1), r3=ma.Sub(mb), r4=ma.Sub(1), r5=ma.Mult(2);
	Mat44 r6 = ma.Mult_ComponentWise(mb), r7 = ma.Mult_ComponentWise(Mat44::IDENTITY);*/
	
	OBJFile f;
	f.ParseOBJFile("Data/cube.obj");
	f.GetModels()[0]->RecalculatePerVertexNormals();

	GLWindowEx::OnCreate();
	SetWindowResolution(windowRes[0], windowRes[1]);
};

void SeasonalWindow::OnDestroy()
{
};