#include "SeasonalWindow.h"
#include "OBJFile.h"
#include "float4.h"
#include "Mat44.h"
#include "AppConfig.h"
#include "strutils.h"
#include <WinUser.h>
#include <iostream>
using namespace std;

const c8* SeasonalWindow::DEFAULT_WINDOW_TITLE = "Seasonal Globe";

SeasonalWindow::SeasonalWindow() : clearColor(Color::BLACK), _leftDown(false), _rightDown(false)
{
	windowRes[0] = DEFAULT_WIDTH;
	windowRes[1] = DEFAULT_HEIGHT;

	SetSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	SetDepthBits(24);
	SetTitle(DEFAULT_WINDOW_TITLE);
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
	
	clearColor = Color::GREY;
	glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
	
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
		GLint params[2];
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

	switch(tolower(key))
	{
	case 'y':
		scn.scaleX += 0.01f;
		break;
	case 'u':
		scn.scaleZ += 0.01f;
		break;
	case 'h':
		scn.scaleX -= 0.01f;
		break;
	case 'j':
		scn.scaleZ -= 0.01f;
		break;

		case 'a': 
			//scn.GetCamera().Rotate(Mat44::BuildRotationMatrix(5, 1,0,0));
			//scn.SetCameraRotation(scn.GetCameraRotation() + 5.0f);
			scn.SetCameraAngle(scn.GetCameraAngle() + 5.0f);
			break;
		case 'z':
			scn.SetCameraAngle(scn.GetCameraAngle() - 5.0f);
			//scn.SetCameraRotation(scn.GetCameraRotation() - 5.0f);
			//scn.GetCamera().Rotate(Mat44::BuildRotationMatrix(-5, 1,0,0));
			break;
		case 't':
			{
				if(!down)
				{
					if(scn.GetPolygonMode() == GL_LINE)
					{
						scn.SetPolygonMode(GL_FILL);
					}
					else
					{
						scn.SetPolygonMode(GL_LINE);
					}
				}
			} break;
		case 'c':
			{
				if(!down)
				{
					if(glIsEnabled(GL_CULL_FACE))
						glDisable(GL_CULL_FACE);
					else
						glEnable(GL_CULL_FACE);
				}
			} break;
		case 'r':
			{
				if(!down)
				{
					scn.SetAutoRotate(!scn.GetAutoRotate());
				}
			} break;
		case 'q':
			{
				if(!down)
					scn.GetTree()->SetDrawLevel(scn.GetTree()->GetDrawLevel()-1);
			} break;
		case 'w':
			{
				if(!down)
					scn.GetTree()->SetDrawLevel(scn.GetTree()->GetDrawLevel()+1);
			} break;
		case 'b':
			{
				if(!down)
					scn.GetTree()->SetAnimationLevel(0);
			}
	}
};

void SeasonalWindow::OnMouseButton(MouseButton button, bool down)
{
	if (down) {
		switch(button) {
		case MBLeft: 
			_leftDown = true;
			break;
		case MBRight:
			_rightDown = true;
			break;
		default:
			break;
		}
	} else {
		switch(button) {
		case MBLeft: 
			_leftDown = false;
			break;
		case MBRight:
			_rightDown = false;
			break;
		default:
			break;
		}
	}
};

void SeasonalWindow::OnMouseMove(i32 x, i32 y)
{
	static i32 temp_x, temp_y;
	if(_leftDown) {
		//scn.GetCamera().Translate( scn.GetCamera().GetDirection() * ((f32)y-(f32)temp_y)*0.05f );
		float3 currentPos = scn.GetCamera().GetPosition();
		currentPos.z( scn.GetCamera().GetDirection().z() * currentPos.z() +  (y-temp_y)*0.05f );
		scn.SetCameraPosition(scn.GetCameraPosition() + (y-temp_y)*0.05f);
		//scn.GetCamera().SetPosition(currentPos);
		//scn._cameraPosition += (y-temp_y)*0.05f;
	}
	if(_rightDown) {
		scn.SetCameraRotation(scn.GetCameraRotation()+(x-temp_x)*0.5f);
		//float3 currentPos = scn.GetCamera().GetDirection();
		//currentPos.z( currentPos.z() +  +(x-temp_x)*0.5f );
		//scn.GetCamera().SetPosition(currentPos);
		//scn.GetCamera().Rotate( Mat44::BuildRotationMatrix( (x-temp_x)*0.5f, 0,1,0));
		//scn._cameraRotation += (x-temp_x)*0.5f;
	}
	temp_x = x;
	temp_y = y;
};

void SeasonalWindow::OnCreate()
{
	AppConfig conf;
	if(conf.ParseConfigFile("Data/ConfigFile.txt"))
		cout << "Parsed config file" << endl;
	else
		cout << "Failed to parse config file" << endl;

	GLWindowEx::OnCreate();
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	scn.Load();

	gameTime.Init();

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