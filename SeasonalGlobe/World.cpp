#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "CylindricalParticleEmitter.h"
#include "SeasonManager.h"
#include "graphics_utils.h"

World::World(void)
	: dtMultiplier(1),
	sceneRotationAxis(0,1,0), _cameraAngle(30.0f), _cameraPosition(-30.0f), // Camera
	_cameraRotation(-357.0f), AutoRotate(false), snowSlowing(false),
	terrainElevation(0.45f, 0.13f, 3.0f), tree(0), polygonMode(GL_FILL), drawLightning(false),
	terrainTextureMergeFactor(0), mergingTerrainTextured(false),
	timeToMergeTextures(8), terrainTextureMergeRuntime(0), mergeDirection(1),
	leafParticleEmitterID(0), snowEmitterID(0), smokeEmitterID(0), grassStaticEmitterID(0) , fireParticleEmitter(0), // Particle Emitters
	directionalLightRotation(0), directionalLightSpeed(0.5f), lightMode(Directional) // Lights
{
}

World::~World(void)
{
	Shutdown();
}

f32 angle=0; const f32 rotationSpeed = 50.0f;
f32 inc=0;

// Draw the reflective pond
void World::reflective_draw(const GameTime &gameTime)
{
	float3 floorScale(0.3f, 0.3f, 0.3f);
	float3 floorPos(2.5f, -0.45f, 3.2f);

	// Disable depth testing, color, OpenGL lighting and texturing. Enable stencil test.
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_2D);

	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);

	// Draw floor into stencil
	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();

	// Re-enable color and depth
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Reflect everything in Y axis (scale by -1 on Y)
	glPushMatrix();
	glScalef(1,-1,1);

	glCullFace(GL_FRONT);
	glEnable(GL_NORMALIZE);

	// Draw tree reflected
	glDisable(GL_LIGHTING);
	tree->Draw(gameTime.GetDeltaTime());

	// Draw fire particles and leaf particles reflected
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	fireParticleEmitter->Draw();
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->Draw();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glDisable(GL_NORMALIZE);
	glCullFace(GL_BACK);

	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	fireParticleEmitter->glUseProgram(0);
	glEnable(GL_LIGHTING);

	glColor4f(1,1,1,1);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Color::WHITE.GetVec());
	Material mat(color(0.2,0.2,0.2,0.5), color(0.7,0.7,0.7,0.5), color(0.0f), 0);
	mat.Activate();

	// Draw floor normally

	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glFrontFace(GL_CW);
	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);

	glDisable(GL_LIGHTING);

	// Draw terrain normally
	DrawTerrain(gameTime);
};

void World::Update(const GameTime &_gameTime)
{
	GameTime gameTime = _gameTime;

	// multiply game time by dtMultipler (to speed up/slow down time)
	gameTime.SetDeltaTime(gameTime.GetDeltaTime() * (f32)dtMultiplier);

	// update timings of seasonal events
	UpdateSceneTimings();

	// merging terrain textures?
	if(mergingTerrainTextured)
	{
		terrainTextureMergeRuntime += gameTime.GetDeltaTime() * mergeDirection; // update merge runtime
		if(terrainTextureMergeRuntime > timeToMergeTextures)
		{
			terrainTextureMergeRuntime = timeToMergeTextures - EPSILON;
		}
		else if(terrainTextureMergeRuntime <= 0)
		{
			terrainTextureMergeRuntime = 0;
		}
		terrainTextureMergeRuntime = fmod(terrainTextureMergeRuntime, timeToMergeTextures);
		terrainTextureMergeFactor = (1.0f / timeToMergeTextures) * terrainTextureMergeRuntime; // texture mix factor between 0 and 1
	}

	// update snow if active
	HemiSphericalParticleEmitter *snowEmitter = particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID);
	if(snowEmitter->IsActive())
	{
		if(snowEmitter->GetActivity() < 1.0f && !snowSlowing)
		{
			snowEmitter->SetActivity(snowEmitter->GetActivity() + 0.005f);
		}
		else if(snowSlowing && snowEmitter->GetActivity()>0)
		{
			snowEmitter->SetActivity(snowEmitter->GetActivity() - 0.005f);
		}
	}

	// update rest of particles
	particleSystem.Update(gameTime);

	// update fire particle emitter
	fireParticleEmitter->UpdateFireParticleEmitter(gameTime);
};

void World::DrawTerrain(const GameTime &gameTime)
{
	// Terrain (floor)
	terrainElevation.elevationRuntime += gameTime.GetDeltaTime() * terrainElevation.shiftDirection;
	if(terrainElevation.elevationRuntime <= 0 && terrainElevation.shiftDirection == Down) { terrainElevation.elevationRuntime=0; terrainElevation.shiftDirection = NoShift; }

	glEnable(GL_NORMALIZE);
	terrainElevation.elevationRuntime = min(terrainElevation.elevationRuntime, terrainElevation.timeToElevateFully); // or =fmod(CurrentTerrainTime, ShiftTime); to loop terrain
	f32 N = (1.0f / terrainElevation.timeToElevateFully) * terrainElevation.elevationRuntime;
	f32 vpos = lerp(0, terrainElevation.maxDisplacementScaleFactor, N);
	f32 CurrentShift = lerp(0, terrainElevation.maxGeometryShiftFactor, N);

	// set shader uniforms (inc lerp factor and terrain shift)
	glDisable(GL_CULL_FACE); 
	terrain.SetPosition(float3(0, terrain.GetPosition().y()-CurrentShift,0));
	terrain.GetShader()->Activate();
	terrain.GetShader()->SetUniform("t", terrainTextureMergeFactor);
	terrain.GetShader()->SetUniform("normalLerpFactor", N);
	terrain.GetShader()->SetUniform("vposmult", vpos);
	terrain.GetShader()->Deactivate();
	terrain.SetXRotation(-90); 
	terrain.SetYRotation(0);
	terrain.SetZRotation(-90); 
	terrain.Draw();  // draw terrain
	terrain.SetPosition(float3(0, terrain.GetPosition().y()+CurrentShift,0));
	glEnable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
};

void World::Draw(const GameTime &_gameTime)
{
	GameTime gameTime = _gameTime;
	gameTime.SetDeltaTime(gameTime.GetDeltaTime() * (f32)dtMultiplier);

	seasonMan.Update(gameTime.GetDeltaTime());

	glLoadIdentity();

	glPushMatrix();

	// This translates everything else in the scene, objects move, not the camera
	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	glRotatef(angle, 0, 1, 0);

	// enable spotlights if applicable
	if(lightMode == Spotlights)
	{
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		glDisable(GL_LIGHTING);
		
		spotlights[0].Activate();
		spotCone.SetXRotation(0);
		spotCone.SetPosition(spotlights[0].GetPosition().ToFloat3());
		spotCone.Draw();

		spotlights[1].Activate();
		spotCone.SetXRotation(90);
		spotCone.SetPosition(spotlights[1].GetPosition().ToFloat3());
		spotCone.Draw();

		spotlights[2].Activate();
		spotCone.SetPosition(spotlights[2].GetPosition().ToFloat3());
		spotCone.Draw();

		spotlights[3].Activate(); 
		spotCone.SetPosition(spotlights[3].GetPosition().ToFloat3());
		spotCone.SetZRotation(180);
		spotCone.Draw();
		spotCone.SetZRotation(0);
	}
	else if(lightMode == Directional) // enable directional light if applicable
	{
		// Directional Light
		directionalLight.Activate();
		f32 radius = 13; float3 pos(0,0,0);
		float3 newPos(pos.x()+radius*sin(directionalLightRotation), pos.y()+radius*cos(directionalLightRotation), pos.z());
		lightSphere.SetPosition(newPos);
		float4 lightSpherePos = lightSphere.GetPosition().ToFloat4();
		directionalLight.SetPositionAndUpdateOpenGL(lightSpherePos);
		directionalLightRotation += directionalLightSpeed * gameTime.GetDeltaTime();
		lightSphere.Draw();
	}

	if(drawLightning) // draw lightning
	{
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glTranslatef(tree->GetPosition().x(), tree->GetPosition().y(), tree->GetPosition().z());
		glTranslatef(-0.25,5,0);
		glScalef(2,2,2);
		glColor4f(0.9451f, 0.9178f, 0,1);
		boltModel.Draw();
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glColor4f(1,1,1,1);
	}

	// Draw pond and terrain
	glPushMatrix();
	reflective_draw(gameTime);
	glPopMatrix();

	// House
	houseModel.Draw();

	// Base
	baseModel.Draw();

	// Draw tree normally
	tree->Draw(gameTime.GetDeltaTime());

	// Particle system
	particleSystem.Draw();

	// Globe
	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	globeSphere.Draw();
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_PLANE0);

	glPopMatrix();

	if(AutoRotate)
	{
		angle += rotationSpeed * gameTime.GetDeltaTime();
		if(angle >= 360) { angle-=360; }
	}
}

void World::ResetWorld()
{
	seasonMan.Reset();

	// RESET TREE
	tree->SetRuntime(0);
	tree->SetTreeDieing(false);
	tree->SetTreeDeathDepth(0);
	tree->SetActive(false);

	snowSlowing = false;

	// RESET LEAVES
	const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
	const Mat44 *LeafMatrices = tree->GetLeafMatrices();
	const u32 LeafCount = tree->GetLeafCount();
	StaticParticleEmitter *leafEmitter = particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID);
	leafEmitter->ResetParticleEmitter();
	for(u32 i=0;i<LeafCount;++i)
	{
		glMatrixMode(GL_MODELVIEW);
		const f32 *leafMat = LeafMatrices[i].GetMatrix();
		for(u32 j=0;j<LEAF_PARTICLES_PER_LEAF_MATRIX;++j)
		{
			leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].pos = float3(leafMat[12], leafMat[13], leafMat[14]);
			leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].size = float3(0.25f);
			leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].rotation_z = randflt(0, 360);
			leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].color = leafEmitter->GetStartColor();
		}
	}
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetStartColor(Color4f(1,1,1,0));
	leafEmitter->SetEndColor(Color4f(1,1,1,1));
	leafEmitter->SetParticlesDieing(false);
	leafEmitter->SetParticlesFadingIn(false);
	leafEmitter->SetParticlesFalling(false);
	leafEmitter->DoColorUpdate(true);
	leafEmitter->SetActive(false);

	fireParticleEmitter->SetRuntime(0);
	fireParticleEmitter->SetBurningState(Igniting);
	fireParticleEmitter->SetBurnLevel(0);
	fireParticleEmitter->SetActive(false);

	terrainElevation.shiftDirection = NoShift;

	particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->ResetParticleEmitter();
	particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->SetActive(false);

	particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->ResetParticleEmitter();
	particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->SetRateOfEmission(500);
	particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->SetActive(false);
};

void World::SetLightingMode(LightingMode m) // need to switch shaders when we change how the scene is lit
{
	lightMode = m;

	if(lightMode == Ambient) // AMBIENT LIGHTS
	{
		houseModel.SetShader(normalMap_AmbientShader);
		baseModel.SetShader(ambientLightShader);
		tree->SetShader(ambientLightShader);
		terrain.SetShader(terrainShaders.Terrain_Displacement_Ambient_Shader);
	}
	else if(lightMode == Directional) // DIRECTIONAL LIGHT
	{
		houseModel.SetShader(normalMap_DirectionalShader);
		baseModel.SetShader(directionalLightShader);
		tree->SetShader(directionalLightShader);
		terrain.SetShader(terrainShaders.Terrain_Displacement_Directional_Shader);
	}
	else if(lightMode == Spotlights) // SPOT LIGHTS
	{ 
		houseModel.SetShader(normalMap_SpotlightsShader);
		baseModel.SetShader(spotlightShader);
		tree->SetShader(spotlightShader);
		terrain.SetShader(terrainShaders.Terrain_Displacement_Spotlights_Shader);
	} 

	SetTreeShadeMode(tree->GetTreeShadeMode());
};

const TreeShadingMode World::GetNextTreeShadeMode() const
{
	switch(tree->GetTreeShadeMode())
	{
	case NonTexturedNonLitWireframe:
		return FlatNonTextured;
	case FlatNonTextured:
		return SmoothNonTextured;
	case SmoothNonTextured:
		return SmoothTextured;
	case SmoothTextured:
		return NormalMappedTextured;
	case NormalMappedTextured:
		return NonTexturedNonLitWireframe;
	}

	return SmoothTextured;
};

void World::SetTreeShadeMode(TreeShadingMode m)
	{
		// Non Textured non lit wireframe
		if(m == NonTexturedNonLitWireframe)
		{
			tree->SetPolygonFillMode(GL_LINE);
			tree->SetTreeShadeMode(0, 0, NonTexturedNonLitWireframe);
		}

		// Flat non textured
		else if(m == FlatNonTextured)
		{
			tree->SetPolygonFillMode(GL_FILL);
			if(lightMode == Ambient)
			{
				treeShaders.Tree_Ambient_Shader->SetUniform("useTextures", false);
				tree->SetTreeShadeMode(treeShaders.Tree_Ambient_Shader, 0, FlatNonTextured);
			}
			else if(lightMode == Directional)
			{
				tree->SetTreeShadeMode(treeShaders.FlatNonTextured_Directional_Shader, 0, FlatNonTextured);
			}
			else if(lightMode == Spotlights)
			{
				tree->SetTreeShadeMode(treeShaders.FlatNonTextured_Spot_Shader, 0, FlatNonTextured);
			}
		}

		// Smooth non textured
		else if(m == SmoothNonTextured)
		{
			tree->SetPolygonFillMode(GL_FILL);
			if(lightMode == Ambient)
			{
				treeShaders.Tree_Ambient_Shader->SetUniform("useTextures", false);
				tree->SetTreeShadeMode(treeShaders.Tree_Ambient_Shader, 0, SmoothNonTextured);
			}
			else if(lightMode == Directional)
			{
				treeShaders.SmoothShaded_Directional_Shader->SetUniform("useTextures", false);
				tree->SetTreeShadeMode(treeShaders.SmoothShaded_Directional_Shader, 0, SmoothNonTextured);
			}
			else if(lightMode == Spotlights)
			{
				treeShaders.SmoothShaded_Spot_Shader->SetUniform("useTextures", false);
				tree->SetTreeShadeMode(treeShaders.SmoothShaded_Spot_Shader, 0, SmoothNonTextured);
			}
		}

		// Smooth textured
		else if(m == SmoothTextured)
		{
			tree->SetPolygonFillMode(GL_FILL);
			if(lightMode == Ambient)
			{
				treeShaders.Tree_Ambient_Shader->SetUniform("useTextures", true);
				tree->SetTreeShadeMode(treeShaders.Tree_Ambient_Shader, barkTexture, SmoothTextured);
			}
			else if(lightMode == Directional)
			{
				treeShaders.SmoothShaded_Directional_Shader->SetUniform("useTextures", true);
				tree->SetTreeShadeMode(treeShaders.SmoothShaded_Directional_Shader, barkTexture, SmoothTextured);
			}
			else if(lightMode == Spotlights)
			{
				treeShaders.SmoothShaded_Spot_Shader->SetUniform("useTextures", true);
				tree->SetTreeShadeMode(treeShaders.SmoothShaded_Spot_Shader, barkTexture, SmoothTextured);
			}
		}
		else if(m == NormalMappedTextured)
		{
			tree->SetTreeShadeMode(normalMap_DirectionalShader, barkTexture, NormalMappedTextured);
		}
	};