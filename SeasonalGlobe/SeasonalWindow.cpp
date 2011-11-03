#include "SeasonalWindow.h"

const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;

SeasonalWindow::SeasonalWindow()
{
	windowRes[0] = DEFAULT_WIDTH;
	windowRes[1] = DEFAULT_HEIGHT;
};

const int* const SeasonalWindow::GetWindowRes() const
{
	return windowRes;
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
	SwapBuffers();
};

void SeasonalWindow::OnIdle()
{
	Redraw();
};

void SeasonalWindow::OnKeyboard(int key, bool down)
{
};

void SeasonalWindow::OnCreate()
{
	GLWindowEx::OnCreate();
	SetWindowResolution(windowRes[0], windowRes[1]);
};

void SeasonalWindow::OnDestroy()
{
};