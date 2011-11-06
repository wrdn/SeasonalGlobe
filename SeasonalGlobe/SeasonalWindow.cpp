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
	f32 matData[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
	Mat44 ma(matData);

	Mat44 mulSelf = ma.Mult(ma);

	float4 vIn[] = 
	{
		float4(5, 12, 1.8, 20.7),
		float4(2, 4.5, 34.5, 0.9)
	};
	float4 mulVM = ma.Mult(*vIn);
	cout << "Original Matrix:\n" << ma << "\nOriginal Vector: " << vIn[0] << "\n\nMatrix-Vector Multiplication Result: "
		<< mulVM << endl;

	float4 vout[2];
	ma.BatchMult(vIn, vout, sizeof(vIn)/sizeof(float4));

	Mat44 mb(matData);
	mb.Identity();
	Mat44 r1 = ma.Add(mb);
	Mat44 r2 = ma.Add(1);
	Mat44 r3 = ma.Sub(mb);
	Mat44 r4 = ma.Sub(1);
	Mat44 r5 = ma.Mult(2);
	Mat44 r6 = ma.Mult_ComponentWise(mb);
	Mat44 r7 = ma.Mult_ComponentWise(Mat44::IDENTITY);
	Mat44 r8 = ma.Transpose();
	cout << ma << endl << r8 << endl;

	OBJFile f;
	f.ParseOBJFile("Data/cube.obj");
	f.GetModels()[0]->RecalculatePerVertexNormals();

	GLWindowEx::OnCreate();
	SetWindowResolution(windowRes[0], windowRes[1]);
};

void SeasonalWindow::OnDestroy()
{
};