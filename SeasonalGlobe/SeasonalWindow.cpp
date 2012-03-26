#include "SeasonalWindow.h"
#include "PointBasedParticleEmitter.h"
#include <WinUser.h>

const c8* SeasonalWindow::DEFAULT_WINDOW_TITLE = "Seasonal Globe";

SeasonalWindow::SeasonalWindow() : _leftDown(false), _rightDown(false), displayHelpMenu(false)
{
	windowRes[0] = DEFAULT_WIDTH;
	windowRes[1] = DEFAULT_HEIGHT;

	SetSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	SetDepthBits(24);
	SetStencilBits(1); // Required to be 1 bit only
	SetTitle(DEFAULT_WINDOW_TITLE);
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

// reset projection matrix (using gluPerspective)
void SeasonalWindow::ResetPerspective() const
{
	glViewport(0,0,windowRes[0],windowRes[1]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)windowRes[0] / (double)windowRes[1],0.3f,200.0);
	glMatrixMode(GL_MODELVIEW);
};

// Display scene
void SeasonalWindow::OnDisplay()
{
	gameTime.Update();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0,0,0,1);

	// draw world
	scn.Draw(gameTime);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// draw current season
	glRasterPos2f(0.5f, 0.9f);
	Printf("Current Season: %s", scn.GetCurrentSeasonString());
	
	// draw current time multiplier
	glRasterPos2f(-0.96f, 0.9f);
	Printf("Current Time Multiplier: %f", scn.GetMultiplier());

	// display help menu if required
	if(displayHelpMenu)
	{
		glRasterPos2f(-0.96f, 0.80f); Printf("Press p to pause time");
		glRasterPos2f(-0.96f, 0.75f); Printf("Press r to set default time multiplier");
		glRasterPos2f(-0.96f, 0.7f);  Printf("Press +/- to increase/decrease time multiplier");
		glRasterPos2f(-0.96f, 0.65f); Printf("Press s to switch lighting mode");
		glRasterPos2f(-0.96f, 0.6f);  Printf("Press t to switch polygon mode (wireframe or fill)");
		glRasterPos2f(-0.96f, 0.55f);  Printf("Press m to switch tree viewing mode");

		glRasterPos2f(-0.96f, 0.45f);  Printf("Press Left/Right or Up/Down arrows to rotate camera");
		glRasterPos2f(-0.96f, 0.4f); Printf("Hold Left Mouse and move mouse to zoom");
		glRasterPos2f(-0.96f, 0.35f);  Printf("Hold Right Mouse and move mouse to rotate");

		glRasterPos2f(-0.96f, 0.25f);  Printf("Press h to hide/display this help menu");
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

// request redraw on idle
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
	
	if(down) return; // key must be up

	switch(tolower(key))
	{
	case 'h':
		{
			displayHelpMenu = !displayHelpMenu;
		} break;
	case 187: //+
		scn.SetDtMultiplier(scn.GetMultiplier() + 0.1);
		break;
	case 189: //-
		scn.SetDtMultiplier(scn.GetMultiplier() - 0.1);
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

// rotate camera when left/right mouse button down and moving mouse
void SeasonalWindow::OnMouseMove(i32 x, i32 y)
{
	static i32 temp_x, temp_y;
	if(_leftDown)
	{
		scn.SetCameraPosition(scn.GetCameraPosition() + (y-temp_y)*0.05f);
	}
	if(_rightDown)
	{
		scn.SetCameraRotation(scn.GetCameraRotation()+(x-temp_x)*0.5f);
	}
	temp_x = x;
	temp_y = y;
};

// load stuff on window creation
void SeasonalWindow::OnCreate()
{
	GLWindowEx::OnCreate();

	alutInit(0,0);

	SetCursor(CRNone); // hide cursor when loading

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

	// load scene
	scn.Load();

	// redisplay cursor
	SetCursor(CRArrow);
	glMatrixMode(GL_PROJECTION); glPopMatrix();   
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	// initialise game time
	gameTime.Init();
};