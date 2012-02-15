#include "SeasonalWindow.h"
#include "PointBasedParticleEmitter.h"
#include <WinUser.h>

const c8* SeasonalWindow::DEFAULT_WINDOW_TITLE = "Seasonal Globe";

SeasonalWindow::SeasonalWindow() : clearColor(Color::BLACK), _leftDown(false), _rightDown(false), displayHelpMenu(false)
{
	windowRes[0] = DEFAULT_WIDTH;
	windowRes[1] = DEFAULT_HEIGHT;

	SetSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	SetDepthBits(24);
	SetStencilBits(1);
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

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0,0,0,1);

	scn.Draw(gameTime);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(0.5f, 0.9f);
	Printf("Current Season: %s", scn.GetCurrentSeasonString());
	
	glRasterPos2f(-0.96f, 0.9f);
	Printf("Current Time Multiplier: %f", scn.GetMultiplier());

	if(displayHelpMenu)
	{
		glRasterPos2f(-0.96f, 0.80f); Printf("Press p to pause time");
		glRasterPos2f(-0.96f, 0.75f); Printf("Press r to set default time multiplier");
		glRasterPos2f(-0.96f, 0.7f);  Printf("Press +/- to increase/decrease time multiplier");
		glRasterPos2f(-0.96f, 0.65f); Printf("Press s to switch lighting mode");
		glRasterPos2f(-0.96f, 0.6f);  Printf("Press t to switch polygon mode (wireframe or fill)");

		glRasterPos2f(-0.96f, 0.5f);  Printf("Press Left/Right or Up/Down arrows to rotate camera");
		glRasterPos2f(-0.96f, 0.45f); Printf("Hold Left Mouse and move mouse to zoom");
		glRasterPos2f(-0.96f, 0.4f);  Printf("Hold Right Mouse and move mouse to rotate");

		glRasterPos2f(-0.96f, 0.3f);  Printf("Press h to hide/display this help menu");
	}
	else
	{
		glRasterPos2f(-0.96f, 0.80f);
		Printf("Press h to display help menu");
	}
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	SwapBuffers();
};

void SeasonalWindow::OnIdle()
{
	scn.Update(gameTime);
	Redraw();
};

void SeasonalWindow::OnKeyboard(i32 key, bool down)
{
	if(key == VK_ESCAPE && down)
	{
		Close();
	}
	
	if(down) return;

	switch(tolower(key))
	{
	case 'h':
		{
			displayHelpMenu = !displayHelpMenu;
		} break;
	case 187: //+
		scn.SetDtMultiplier(scn.GetMultiplier() + 0.1);
		//scn.GetSeasonManager()->SetTimePerSeason( scn.GetSeasonManager()->GetTimePerSeason() + 0.1f );
		break;
	case 189: //-
		scn.SetDtMultiplier(scn.GetMultiplier() - 0.1);
		//scn.GetSeasonManager()->SetTimePerSeason( scn.GetSeasonManager()->GetTimePerSeason() - 0.1f );
		break;
	case 'p':
		{
			scn.SetDtMultiplier(0);
		} break;
	case 'r':
		{
			scn.SetDtMultiplier(1);
		} break;
	case 37:
		{
			scn.SetCameraRotation(scn.GetCameraRotation() - 5);
			break;
		}
	case 39:
		{
			scn.SetCameraRotation(scn.GetCameraRotation() + 5);
			break;
		}
	case 38:
		{
			scn.SetCameraAngle(scn.GetCameraAngle() - 5);
			break;
		}
	case 40:
		{
			scn.SetCameraAngle(scn.GetCameraAngle() + 5);
			break;
		}
	case 109:
		{
			scn.SetTreeShadeMode(scn.GetNextTreeShadeMode());
		} break;
	case 115:
		{
			// Order: Directional - Spotlight - Ambient - Directional
			const LightingMode lm = scn.GetLightingMode();
			switch(lm)
			{
			case Directional: { scn.SetLightingMode(Spotlights); break; }
			case Spotlights: { scn.SetLightingMode(Ambient); break; }
			case Ambient: { scn.SetLightingMode(Directional); break; }
			}
		} break;
	case 't':
		{
			scn.SetPolygonMode(scn.GetNextPolygonMode(scn.GetPolygonMode()));
		};
	}





	/*else if(key == VK_RETURN && !down)
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
	}*/
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

		/*Camera2 &cam = scn.GetCamera();
		f32 ychange = (f32)y-(f32)temp_y;

		Mat44 rotationMatrix = Mat44::BuildRotationMatrix(ychange, cam.right.x(), cam.right.y(), cam.right.z());
		cam.dir = rotationMatrix.Mult(cam.dir.ToFloat4()).ToFloat3();
		cam.up = rotationMatrix.Mult(cam.up.ToFloat4()).ToFloat3();
		*/
		//Mat44 m = Mat44::BuildRotationMatrix( changeInY, 0, 1, 0);
		//cam.dir = m.Mult(cam.dir.ToFloat4()).ToFloat3();
		//cam.up = m.Mult(cam.up.ToFloat4()).ToFloat3();


		scn.SetCameraPosition(scn.GetCameraPosition() + (y-temp_y)*0.05f);

		//scn.GetCamera().Translate( scn.GetCamera().GetDirection() * ((f32)y-(f32)temp_y)*gameTime.GetDeltaTime() );
		//const f32 speed = 0.1f;
		//const f32 d = ((f32)y-(f32)temp_y);
		//scn.GetCamera().SetRadius ( max(0.1, scn.GetCamera().GetRadius() + d*speed) );
		//scn.GetCamera().SetRadius( scn.GetCamera().GetRadius() * ((f32)y-(f32)temp_y)*gameTime.GetDeltaTime() );
		//scn.GetCamera().Translate( scn.GetCamera().GetDirection() * ((f32)y-(f32)temp_y)*0.05f );
		/*float3 currentPos = scn.GetCamera().GetPosition();
		currentPos.z( scn.GetCamera().GetDirection().z() * currentPos.z() +  (y-temp_y)*0.05f );
		scn.SetCameraPosition(scn.GetCameraPosition() + (y-temp_y)*0.05f);*/
		//scn.GetCamera().SetPosition(currentPos);
		//scn._cameraPosition += (y-temp_y)*0.05f;


	}
	if(_rightDown) {

		//Camera2 &cam = scn.GetCamera();


		scn.SetCameraRotation(scn.GetCameraRotation()+(x-temp_x)*0.5f);

		//const f32 d = ((f32)x - (f32)temp_x);
		//scn.GetCamera().SetTheta ( scn.GetCamera().GetTheta() + d*0.1f );
		//scn.GetCamera().Rotate( Mat44::BuildRotationMatrix( (x-temp_x)*0.5f, 0,1,0 ) );
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
	GLWindowEx::OnCreate();
	SetCursor(CRNone);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	SetTitle(DEFAULT_WINDOW_TITLE);
	SetWindowResolution(windowRes[0], windowRes[1]);

	// Draw Loading . . .
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); glLoadIdentity();
	glRasterPos2f(-0.15f, 0);
	Printf("Loading . . .");
	SwapBuffers();

	scn.Load();

	SetCursor(CRArrow);
	glMatrixMode(GL_PROJECTION); glPopMatrix();   
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	gameTime.Init();
};

void SeasonalWindow::OnDestroy()
{
};