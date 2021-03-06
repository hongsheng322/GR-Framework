#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"
#include "FontType.h"
#include "LoadTextData.h"
#include "EnemyManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"
#include "HardwareAbstraction\Keyboard.h"
#include "BuildingManager.h" 
#include "../Source/Sound_Engine.h"
#include "InteractionEntity.h"

#include <iostream>
#include "RenderHelper.h"

#include "Minimap.h"

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
	: theMiniMap(NULL) 
{
	_sceneMgr->AddScene("Start", this);
	
}

SceneText::~SceneText()
{
	CMinimap::Destroy();
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Shadow.vertexshader", "Shader//Shadow.fragmentshader");

	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled[0]");
	currProg->AddUniform("colorTexture[0]");
	currProg->AddUniform("colorTextureEnabled[1]");
	currProg->AddUniform("colorTexture[1]");
	currProg->AddUniform("colorTextureEnabled[2]");
	currProg->AddUniform("colorTexture[2]");
	currProg->AddUniform("colorTextureEnabled[3]");
	currProg->AddUniform("colorTexture[3]");
	currProg->AddUniform("colorTextureEnabled[4]");
	currProg->AddUniform("colorTexture[4]");
	currProg->AddUniform("colorTextureEnabled[5]");
	currProg->AddUniform("colorTexture[5]");
	currProg->AddUniform("colorTextureEnabled[6]");
	currProg->AddUniform("colorTexture[6]");
	currProg->AddUniform("colorTextureEnabled[7]");
	currProg->AddUniform("colorTexture[7]");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");

	currProg->AddUniform("shadowMap");
	currProg->AddUniform("lightDepthMVP");
	
	GraphicsManager::GetInstance()->m_gPassShaderID = LoadShaders("Shader//GPass.vertexshader", "Shader//GPass.fragmentshader");


	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE_ENABLED] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTextureEnabled[0]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTexture[0]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE_ENABLED1] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTextureEnabled[1]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE1] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTexture[1]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE_ENABLED2] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTextureEnabled[2]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE2] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTexture[2]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE_ENABLED3] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTextureEnabled[3]");
	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_SHADOW_COLOR_TEXTURE3] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "colorTexture[3]");

	GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_LIGHT_DEPTH_MVP_GPASS] =
		glGetUniformLocation(GraphicsManager::GetInstance()->m_gPassShaderID, "lightDepthMVP");
	GraphicsManager::GetInstance()->m_lightDepthFBO.Init(1024, 1024);

	//Add this for tomorrow :)
	////Fog 
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_COLOR] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogParam.color");
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_START] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogParam.start");
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_END] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogParam.end");
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_DENSITY] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogParam.density");
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_TYPE] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogParam.type");
	//GraphicsManager::GetInstance()->gPass_params[GraphicsManager::GPASS_UNIFORM_TYPE::U_FOG_ENABLED] = 
	//	glGetUniformLocation(GraphicsManager::GetInstance()->, "fogparam.enabled");

	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID[0] = LoadTGA("Image//grass_darkgreen.tga");

	MeshBuilder::GetInstance()->GenerateQuad("GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("GRASS_LIGHTGREEN")->textureID[0] = LoadTGA("Image//grass_lightgreen.tga");

	//Building Meshes
	MeshBuilder::GetInstance()->GenerateOBJ("core", "OBJ//core.obj");
	MeshList::GetInstance()->GetMesh("core")->textureID[0] = LoadTGA("Image//Green.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("wall", "OBJ//cube.obj");
	MeshList::GetInstance()->GetMesh("wall")->textureID[0] = LoadTGA("image//wall.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("cover", "OBJ//cube.obj"); //remember to change obj
	MeshBuilder::GetInstance()->GenerateOBJ("floor", "OBJ//cube.obj"); //remember to change texture
	MeshList::GetInstance()->GetMesh("floor")->textureID[0] = LoadTGA("Image//wood.tga");

	//Equipment Meshes
	MeshBuilder::GetInstance()->GenerateOBJ("Turret", "OBJ//turret.obj");
	MeshBuilder::GetInstance()->GenerateOBJ("Healing Station", "OBJ//healStation.obj");
	MeshList::GetInstance()->GetMesh("Healing Station")->textureID[0] = LoadTGA("Image//Equipment//Heal_Active.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Floor Spike", "OBJ//spike.obj");
	MeshList::GetInstance()->GetMesh("Floor Spike")->textureID[0] = LoadTGA("Image//Equipment//Spike.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Shield", "OBJ//equipmentCube.obj"); //remember to change obj

	//Fatigue Mesh
	MeshBuilder::GetInstance()->GenerateQuad("Fatigue", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Normal.tga");

	//Calendar Mesh
	MeshBuilder::GetInstance()->GenerateQuad("sunMoon", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("sunMoon")->textureID[0] = LoadTGA("Image//sunMoon.tga");
	MeshBuilder::GetInstance()->GenerateQuad("calendar", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("calendar")->textureID[0] = LoadTGA("Image//wood.tga");

	MeshBuilder::GetInstance()->GenerateQuad("ammoFeedback", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("ammoFeedback")->textureID[0] = LoadTGA("Image//ammo.tga");
	MeshBuilder::GetInstance()->GenerateQuad("unlockFeedback", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("unlockFeedback")->textureID[0] = LoadTGA("Image//unlock.tga");
	//Shop
	/*MeshBuilder::GetInstance()->GenerateQuad("Shop", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("Shop")->textureID[0] = LoadTGA("Image//towertab.tga");*/

	//Enemy Mesh
	MeshBuilder::GetInstance()->GenerateOBJ("Cuck", "OBJ//Cuck.obj");
	MeshList::GetInstance()->GetMesh("Cuck")->textureID[0] = LoadTGA("Image//Red.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Ruck", "OBJ//cube.obj");
	MeshList::GetInstance()->GetMesh("Ruck")->textureID[0] = LoadTGA("Image//Yellow.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Tuck", "OBJ//Cuck.obj");
	MeshList::GetInstance()->GetMesh("Tuck")->textureID[0] = LoadTGA("Image//Green.tga");
	//MeshBuilder::GetInstance()->GenerateOBJ("Luck", "OBJ//cube.obj");
	MeshBuilder::GetInstance()->GenerateOBJ("Auck", "OBJ//cube.obj");
	MeshBuilder::GetInstance()->GenerateOBJ("Buck", "OBJ//player.obj");
	MeshList::GetInstance()->GetMesh("Buck")->textureID[0] = LoadTGA("Image//PLayer.tga");

	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 1, 1), 24, 24, 1.f);
	MeshList::GetInstance()->GetMesh("sphere")->textureID[0] = LoadTGA("Image//Yellow.tga");
	MeshBuilder::GetInstance()->GenerateSphere("blood", Color(1, 0, 0), 24, 24, 1);
	MeshBuilder::GetInstance()->GenerateSphere("explosion", Color(1, 1, 0), 24, 24, 1);
	generatorCoreHealthBar = MeshBuilder::GetInstance()->GenerateQuad("generatorCoreHealthBar", Color(1, 0, 0), 1.f);
	playerHealthBar = MeshBuilder::GetInstance()->GenerateQuad("playerHealthBar", Color(1, 0.64706f, 0), 1.f);
	wireFrameBox = MeshBuilder::GetInstance()->GenerateQuad("wireFrameBox", Color(1, 0, 0), 1.f);

	// Building,Equipment,Weapon UI
	BEW_UI = MeshBuilder::GetInstance()->GenerateQuad("pistol", Color(1, 1, 1), 1.0f);
	BEW_UI->textureID[0] = LoadTGA("Image//weapon//pistol.tga");

	redquad = MeshBuilder::GetInstance()->GenerateQuad("redquad", Color(1, 1, 1), 1.0f);
	redquad->textureID[0] = LoadTGA("Image//Red.tga");

	//Minimap
	theMiniMap = Create::Minimap();
	theMiniMap->SetTarget(MeshBuilder::GetInstance()->GenerateQuad("miniMapTarget", Color(1, 1, 1), 0.0625f));
	theMiniMap->GetTarget()->textureID[0] = LoadTGA("Image//target.tga");
	theMiniMap->SetBackground(MeshBuilder::GetInstance()->GenerateQuad("miniMap", Color(1, 1, 1), 1.f));
	theMiniMap->GetBackground()->textureID[0] = LoadTGA("Image//grass_lightgreen.tga");
	theMiniMap->SetAvatar(MeshBuilder::GetInstance()->GenerateQuad("MINIMAPAVATAR", Color(1, 1, 1), 0.125f));
	theMiniMap->GetAvatar()->textureID[0] = LoadTGA("Image//Avatar.tga");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, MAX_CELLS * CELL_SIZE, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	currProg->UpdateInt("numLights", 2);
	currProg->UpdateInt("textEnabled", 0);

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	//Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball
	//GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	//aCube->SetCollider(true);
	//aCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//groundEntity = Create::Ground("GRASS_DARKGREEN", "GRASS_LIGHTGREEN");
	//Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	/*SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");*/

	// Customise the ground entity
	//grid renders more quad, doesnt change quad texcoord
	//groundEntity->SetPosition(Vector3(CELL_SIZE * MAX_CELLS / 2, 0, -CELL_SIZE * MAX_CELLS / 2));
	//groundEntity->SetScale(Vector3(CELL_SIZE * MAX_CELLS, CELL_SIZE * MAX_CELLS, 1.f));
	//groundEntity->SetGrids(Vector3(1.f, 1.0f, 1.f));
	ground = MeshBuilder::GetInstance()->GenerateGround("ground", Color(1, 1, 1), 1.f);
	ground->textureID[0] = LoadTGA("Image//grass_lightgreen.tga");
	background = MeshBuilder::GetInstance()->GenerateGround("background", Color(1, 1, 1), 1.f);
	background->textureID[0] = LoadTGA("Image//water.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("yee", "OBJ//yee.obj");
	MeshList::GetInstance()->GetMesh("yee")->textureID[0] = LoadTGA("Image//yee.tga");


	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 5; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}

	Player::GetInstance()->Init();
	camera = new TopDownCamera();
	fpscamera = new FPSCamera();
	Player::GetInstance()->AttachCamera(camera);
	GraphicsManager::GetInstance()->AttachCamera(camera);

	Shop = MeshBuilder::GetInstance()->GenerateQuad("Shop", Color(1, 1, 1), 1.0f);
	Shop->textureID[0] = LoadTGA("Image//towertab.tga");

	Shop2 = MeshBuilder::GetInstance()->GenerateQuad("Shop2", Color(1, 1, 1), 1.0f);
	Shop2->textureID[0] = LoadTGA("Image//defensetab.tga");

	core = Player::GetInstance()->core;

	Constrain = MeshBuilder::GetInstance()->GenerateQuad("Constrain", Color(1, 1, 1), 1.0f);
	Constrain->textureID[0] = LoadTGA("Image//Return_to_the_playzone.tga");

	MainMenu = MeshBuilder::GetInstance()->GenerateQuad("Scene_Pause", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("Scene_Pause")->textureID[0] = LoadTGA("Image//Menu/Pause.tga");

	Arrow = MeshBuilder::GetInstance()->GenerateQuad("ARROW", Color(1, 1, 1), 1.f);
	MeshList::GetInstance()->GetMesh("ARROW")->textureID[0] = LoadTGA("Image//Menu/Arrow.tga");


	//light testing
	light_depth_mesh = MeshBuilder::GetInstance()->GenerateQuad("light_depth_mewsh", Color(1, 0, 1), 1);
	light_depth_mesh->textureID[0] = GraphicsManager::GetInstance()->m_lightDepthFBO.GetTexture();
	//light_depth_mesh->textureID[0] = LoadTGA("Image//calibri.tga");

	//Hello->play2D("Image//Hello.mp3", GL_TRUE);
	isDay = true;
	dayDuration = 180.f;
	time = dayDuration;
	calendarTime = dayDuration;
	noOfDays = 1;
	ghostPos.SetZero();
	ghostScale.SetZero();
	//CSoundEngine::GetInstance()->playthesound("HELLO", 0.2f);
	Math::InitRNG();
	pause = false;

	CSoundEngine::GetInstance()->PlayBackground("DAY");
}

void SceneText::Update(double dt)
{
	if (!pause) {
		float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
		float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
		float fontSize = 25.0f;
		float halfFontSize = fontSize / 2.0f;
		static float angle = 0;

		if (KeyboardController::GetInstance()->IsKeyDown(VK_OEM_PLUS))
			dt *= 10.f;

		for (int i = 0; i < 6; ++i)
		{
			textObj[i]->SetPosition(Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f));
		}
		// Update our entities
		EntityManager::GetInstance()->Update(dt);

		if (isDay) {
			if (EnemyManager::GetInstance()->active) {
				EnemyManager::GetInstance()->End();
				CSoundEngine::GetInstance()->StopBackground();
				CSoundEngine::GetInstance()->PlayBackground("DAY");
				lights[0]->position.Set(0, MAX_CELLS * CELL_SIZE, 0);
				angle = 0;
			}
		}
		else {
			if (!EnemyManager::GetInstance()->active) {
				EnemyManager::GetInstance()->Init();
				CSoundEngine::GetInstance()->StopBackground();
				CSoundEngine::GetInstance()->PlayBackground("NIGHT");
				lights[0]->position.Set(0, MAX_CELLS * CELL_SIZE, 0);
				lights[0]->color.Set(0.25, 0.25, 0.5);
			}
			EnemyManager::GetInstance()->Update(dt);
		}

		// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
		if (KeyboardController::GetInstance()->IsKeyDown('1'))
			glEnable(GL_CULL_FACE);
		if (KeyboardController::GetInstance()->IsKeyDown('2'))
			glDisable(GL_CULL_FACE);
		if (KeyboardController::GetInstance()->IsKeyDown('3'))
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (KeyboardController::GetInstance()->IsKeyDown('4'))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (KeyboardController::GetInstance()->IsKeyDown('5'))
		{
			lights[0]->type = Light::LIGHT_POINT;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('6'))
		{
			lights[0]->type = Light::LIGHT_DIRECTIONAL;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('7'))
		{
			lights[0]->type = Light::LIGHT_SPOT;
		}

		//==========================================================================Light Feature=====================================================//
		float speed = 180 / dayDuration;
		Mtx44 rotate;
		rotate.SetToRotation(speed * (float)dt, 1, 0, 0);
		Vector3 pos(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z);
		lights[0]->position = rotate * lights[0]->position;

		//gradually changes r g b values to simulate lighting
		static double r = 1;
		static double g = 0.6;
		static double b = 0;
		angle += speed* dt;
		static bool change = false;
		if (isDay) {
			lights[0]->color.Set(r, g, b);
			if (b > 0)
				b = Math::Min((float)b, 1.f);
			else
				b = Math::Max((float)b, 0.f);
			if (!change) {
				g += dt / dayDuration * 2 * 0.4;
				if (g > 0.7)
					b += dt / dayDuration * 16;
				if (g >= 1)
					change = true;
			}
			else {
				if (angle > 140) {
					if (g > 0.6)
						g -= dt / dayDuration * 16;
					if (g > 0.7)
						if (b > 0)
							b -= dt / dayDuration * 16;
				}
			}
		}
		else {
			change = false;
		}

		// Debug purpose - changing to day / night
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F3) && isDay)
		{
			isDay = false;
			time = dayDuration;
			calendarTime = 0.0;
		}
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F4) && !isDay)
		{
			isDay = true;
			time = dayDuration;
			calendarTime = dayDuration;
			noOfDays++;
			core->SetHealth(core->GetHealth() - 100);
		}
		if (core->GetHealth() <= 0)
		{
			core->SetIsDone(true);
			SceneManager::GetInstance()->SetActiveScene("Lose");
		}
		if (noOfDays >= 6)
			SceneManager::GetInstance()->SetActiveScene("Win");
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F6))
		{
			Player::GetInstance()->fatigue = Player::FATIGUE::TIRED;
			MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Tired.tga");
		}
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F7))
		{
			Player::GetInstance()->fatigue = Player::FATIGUE::NORMAL;
			MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Normal.tga");
		}
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F8))
		{
			Player::GetInstance()->fatigue = Player::FATIGUE::ENERGETIC;
			MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Energetic.tga");
		}
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F9))
			Player::GetInstance()->SetSlept(false);
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F10))
			Player::GetInstance()->SetSlept(true);
		if (KeyboardController::GetInstance()->IsKeyPressed(VK_F11))
			SceneManager::GetInstance()->SetActiveScene("Lose");

		//day night shift
		time -= dt;
		calendarTime -= dt;
		if (calendarTime <= -180.f)
			calendarTime = dayDuration;
		if ((time <= 0.001 || Player::GetInstance()->GetSlept()) && isDay)
		{
			time = dayDuration;
			isDay = false;
		}
		//if a day(day and night) past,but no sleep
		if (!isDay && !Player::GetInstance()->GetSlept() && time <= 0.002)
		{
			switch (Player::GetInstance()->fatigue)
			{
			case Player::FATIGUE::ENERGETIC:
				Player::GetInstance()->fatigue = Player::FATIGUE::NORMAL;
				MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Normal.tga");
				break;
			case Player::FATIGUE::NORMAL:
				Player::GetInstance()->fatigue = Player::FATIGUE::TIRED;
				MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Tired.tga");
				break;
			}
		}
		//got sleep
		if (Player::GetInstance()->GetSlept())
		{
			switch (Player::GetInstance()->fatigue)
			{
			case Player::FATIGUE::TIRED:
				Player::GetInstance()->fatigue = Player::FATIGUE::NORMAL;
				MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Normal.tga");
				break;
			case Player::FATIGUE::NORMAL:
				Player::GetInstance()->fatigue = Player::FATIGUE::ENERGETIC;
				MeshList::GetInstance()->GetMesh("Fatigue")->textureID[0] = LoadTGA("Image//Fatigue//Energetic.tga");
				break;
			}
			calendarTime = 0.0;
			Player::GetInstance()->SetSlept(false);
		}
		if (time <= 0.001 && !isDay)
		{
			time = dayDuration;
			isDay = true;
			noOfDays++;
		}

		//convert mouse pos on window onto world
		double mouseX, mouseY;
		MouseController::GetInstance()->GetMousePosition(mouseX, mouseY);
		MouseController::GetInstance()->UpdateMousePosition(mouseX, Application::GetInstance().GetWindowHeight() - mouseY);
		MouseController::GetInstance()->GetMousePosition(mouseX, mouseY);

		float windowWidth = (float)Application::GetInstance().GetWindowWidth();
		float windowHeight = (float)Application::GetInstance().GetWindowHeight();
		//for minimap icon to rotate
		Vector3 Up_Direction = Vector3(windowWidth / 2.f, windowHeight, 0.f) - Vector3(windowWidth / 2.f, windowHeight / 2.f, 0.f);
		Vector3 playerMouse_Direction = Vector3((float)mouseX, (float)mouseY, 0.f) - Vector3(windowWidth / 2.f, windowHeight / 2.f, 0.f);

		try
		{
			playerMouse_Direction.Normalize();
			float angle = Math::RadianToDegree(acosf(playerMouse_Direction.Dot(Up_Direction) / (playerMouse_Direction.Length() * Up_Direction.Length())));
			if (playerMouse_Direction.x < 0)
				angle = -angle;
			//std::cout << "angle: " << angle << std::endl;
			CMinimap::GetInstance()->SetAngle(angle);

			//Set & Update wireFrameBox's position & scale
			{
				Vector3 pPos = Player::GetInstance()->GetPosition();
				int x = (int)(pPos.x / CELL_SIZE);
				int z = (int)(pPos.z / CELL_SIZE);

				// Buildings except floor
				if (Player::GetInstance()->GetIsBuilding() &&
					Player::GetInstance()->GetCurrentBuilding() != BuildingEntity::BUILDING_FLOOR)
				{
					// Up
					if (angle >= -53.f && angle <= 53.f)
					{
						ghostPos.Set(x * CELL_SIZE + CELL_SIZE / 2, 1, z * CELL_SIZE);
						ghostScale.Set(CELL_SIZE, 10, 2);
					}
					// Left
					else if (angle >= -127.f && angle <= -53.f)
					{
						ghostPos.Set(x * CELL_SIZE, 1, z * CELL_SIZE + CELL_SIZE / 2);
						ghostScale.Set(2, 10, CELL_SIZE);
					}
					// Right
					else if (angle >= 53.f && angle <= 127.f)
					{
						ghostPos.Set(x * CELL_SIZE + CELL_SIZE, 1, z * CELL_SIZE + CELL_SIZE / 2);
						ghostScale.Set(2, 10, CELL_SIZE);
					}
					// Down
					else if ((angle >= -180.f && angle <= -127.f) || (angle >= 127.f && angle <= 180.f))
					{
						ghostPos.Set(x * CELL_SIZE + CELL_SIZE / 2, 1, z * CELL_SIZE + CELL_SIZE);
						ghostScale.Set(CELL_SIZE, 10, 2);
					}
				}
				// Floor and Equipment
				else if ((Player::GetInstance()->GetIsBuilding() &&
					Player::GetInstance()->GetCurrentBuilding() == BuildingEntity::BUILDING_FLOOR) ||
					Player::GetInstance()->GetIsEquipment())
				{
					ghostPos.Set(x * CELL_SIZE + CELL_SIZE / 2, 1, z * CELL_SIZE + CELL_SIZE / 2);
					ghostScale.Set(CELL_SIZE, 10, CELL_SIZE);
				}
				//Weapon
				else
				{
					ghostPos.SetZero();
					ghostScale.SetZero();
				}
			}

		}
		catch (DivideByZero)
		{
			std::cout << "Cannot move mouse to center, divide by zero(Normalize for minimap icon to rotate)" << std::endl;
		}

		//if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
		//{
		//	std::cout << "Right Mouse Button was released!" << std::endl;
		//}
		//if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
		//{
		//	std::cout << "Middle Mouse Button was released!" << std::endl;
		//}
		//if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
		//{
		//	std::cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << std::endl;
		//}
		//if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
		//{
		//	std::cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << std::endl;
		//}
		// <THERE>
		switch (Player::GetInstance()->CurrentWeaponID())
		{
		case 1:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//pistol.tga");
			break;
		case 2:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//rifle.tga");
			break;
		case 3:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//sniper.tga");
			break;
		case 4:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//nailgun.tga");
			break;
		case 5:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//rocketlauncher.tga");
			break;
		case 6:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//machinegun.tga");
			break;
		case 7:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//railgun.tga");
			break;
		case 8:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//gravitygun.tga");
			break;
		case 9:
			BEW_UI->textureID[0] = LoadTGA("Image//weapon//chickengun.tga");
			break;
		}
		// Update the player position and other details based on keyboard and mouse inputs
		Player::GetInstance()->Update(dt);

		//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

		GraphicsManager::GetInstance()->UpdateLights(dt);

		// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
		// Eg. FPSRenderEntity or inside RenderUI for LightEntity
		std::ostringstream ss;
		ss << std::fixed;
		ss.precision(3);
		float fps = (float)(1.f / dt);
		ss << "FPS: " << fps;
		textObj[0]->SetText(ss.str());

		// Update the player position into textObj[2]
		ss.str("");
		ss.precision(1);
		ss << Player::GetInstance()->CurrentWeaponMagRound() << "/" << Player::GetInstance()->CurrentWeaponTotalRound();//Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f)
		textObj[1]->SetPosition(Vector3(halfWindowWidth - 100, -halfWindowHeight + fontSize + halfFontSize, 0.0f));
		textObj[1]->SetText(ss.str());

		ss.str("");
		ss.precision(0);
		switch (noOfDays)
		{
		case 1:
			ss << noOfDays << "st";
			break;
		case 2:
			ss << noOfDays << "nd";
			break;
		case 3:
			ss << noOfDays << "rd";
			break;
		default:
			ss << noOfDays << "th";
			break;
		}
		textObj[2]->SetPosition(Vector3(-20.f, halfWindowHeight * 0.865f, 0.0f));
		textObj[2]->SetScale(Vector3(40, 40, 40));
		textObj[2]->SetText(ss.str());

		ss.str("");
		ss << "Material: " << Player::GetInstance()->GetMaterial();
		textObj[3]->SetPosition(Vector3(-halfWindowWidth, -halfWindowHeight + fontSize * 1 + halfFontSize, 0.0f));
		textObj[3]->SetText(ss.str());

		ss.str("");
		if (isDay)
		{
			if (Player::GetInstance()->GetIsBuilding())
			{
				switch (Player::GetInstance()->GetCurrentBuilding())
				{
				case BuildingEntity::BUILDING_WALL:
					ss << "Current Building: Wall";
					break;
				case BuildingEntity::BUILDING_COVER:
					ss << "Current Building: Cover";
					break;
				case BuildingEntity::BUILDING_FLOOR:
					ss << "Current Building: Floor";
					break;
				}
			}
			else if (Player::GetInstance()->GetIsEquipment())
			{
				switch (Player::GetInstance()->GetCurrentEquipment())
				{
				case EquipmentEntity::EQUIPMENT_TURRET:
					ss << "Current Equipment: Turret";
					break;
				case EquipmentEntity::EQUIPMENT_HEALING_STATION:
					ss << "Current Equipment: Healing Station";
					break;
				case EquipmentEntity::EQUIPMENT_FLOOR_SPIKE:
					ss << "Current Equipment: Floor Spike";
					break;
				}
			}
			else if (Player::GetInstance()->GetIsWeapon())
			{
				switch (Player::GetInstance()->GetCurrentWeapon())
				{
				case 1:
					ss << "Current Weapon: ";
					break;
				case 2:
					ss << "Current Weapon: ";
					break;
				case 3:
					ss << "Current Weapon: ";
					break;
				case 4:
					ss << "Current Weapon: ";
					break;
				}
			}
		}
		textObj[4]->SetPosition(Vector3(-halfWindowWidth, -halfWindowHeight + fontSize * 2 + halfFontSize, 0.0f));
		textObj[4]->SetText(ss.str());

		Delay += (float)dt;
		if (Delay > 0.5f)
			Delay = 0.5f;
		if (KeyboardController::GetInstance()->IsKeyDown('P') && Delay >= ButtonCooldown)
		{
			pause = true;
		}

		if (KeyboardController::GetInstance()->IsKeyReleased('B') && !Render_Quad)
		{
			Render_Quad = true;
			std::cout << "Shop Rendered" << std::endl;
		}
		else if (KeyboardController::GetInstance()->IsKeyReleased('B') && Render_Quad)
		{
			Render_Quad = false;
			std::cout << "Shop not Rendered" << std::endl;
		}
	}
	else {
		 //run the pause update
		Delay += (float)dt;


		if (Delay > 0.5f)
		{
			Delay = 0.5f;
		}

		if (KeyboardController::GetInstance()->IsKeyDown(VK_UP) && Delay >= ButtonCooldown ||
			KeyboardController::GetInstance()->IsKeyDown('W') && Delay >= ButtonCooldown)
		{
			if (SelectedOptions == Return)
			{
				SelectedOptions = Exit_to_Main_Menu;
			}
			else if (SelectedOptions == Exit_to_Main_Menu)
			{
				SelectedOptions = Return;
			}

			Delay = 0.0f;
		}

		if (KeyboardController::GetInstance()->IsKeyDown(VK_DOWN) && Delay >= ButtonCooldown ||
			KeyboardController::GetInstance()->IsKeyDown('S') && Delay >= ButtonCooldown)
		{
			if (SelectedOptions == Exit_to_Main_Menu)
			{
				SelectedOptions = Return;
			}
			else if (SelectedOptions == Return)
			{
				SelectedOptions = Exit_to_Main_Menu;
			}

			Delay = 0.0f;
		}

		if (KeyboardController::GetInstance()->IsKeyDown(VK_RETURN) && Delay >= ButtonCooldown)
		{
			if (SelectedOptions == Return)
			{
				pause = false;
			}
			else if (SelectedOptions == Exit_to_Main_Menu)
			{
				SceneManager::GetInstance()->SetActiveScene("MainMenu");
				std::cout << "Main Menu Selected" << std::endl;
			}

			Delay = 0.0f;
		}
		/*if (KeyboardController::GetInstance()->IsKeyDown('P'))
		{
		SceneManager::GetInstance()->SetActiveScene("Start");
		std::cout << "Return to Game" << std::endl;
		}
		*/
		switch (SelectedOptions)
		{
		case Return:
			b_Return = true;
			b_Exit_to_Main_Menu = false;
			break;
		case Exit_to_Main_Menu:
			b_Return = false;
			b_Exit_to_Main_Menu = true;
			break;
		}


		if (b_Return)
		{
			arrowPosition = (Vector3((float)Application::GetInstance().GetWindowWidth() / 7.2f,
				(float)Application::GetInstance().GetWindowHeight() / 3.875f, 2.f));
		}
		if (b_Exit_to_Main_Menu)
		{
			arrowPosition = (Vector3((float)Application::GetInstance().GetWindowWidth() / 7.2f,
				(float)Application::GetInstance().GetWindowHeight() / 8.5f, 2.f));
		}
	}
}

void SceneText::Render()
{
	if (!pause) {
		//******************************* PRE RENDER PASS *************************************
		RenderPassGPass();
		//******************************* MAIN RENDER PASS ************************************
		RenderPassMain();
	}
	else {
		GraphicsManager* g = GraphicsManager::GetInstance();
		MS& ms = GraphicsManager::GetInstance()->GetModelStack();

		g->m_renderPass = g->RENDER_PASS_MAIN;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Application::GetInstance().GetWindowWidth(),
			Application::GetInstance().GetWindowHeight());
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(g->GetActiveShader()->GetProgramID());
		//pass light depth texture
		g->m_lightDepthFBO.BindForReading(GL_TEXTURE8);
		glUniform1i(g->GetActiveShader()->GetUniform("shadowMap"), 8);
		//... old stuffs

		GraphicsManager::GetInstance()->UpdateLightUniforms();

		GraphicsManager::GetInstance()->SetPerspectiveProjection(60.f, (float)(Application::GetInstance().m_window_width) / (float)(Application::GetInstance().m_window_height), 0.1f, 10000.0f);
		GraphicsManager::GetInstance()->AttachCamera(camera);

		int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
		int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;

		GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
		GraphicsManager::GetInstance()->DetachCamera();

		ms.PushMatrix();
		//ms.Translate(-halfWindowWidth, -halfWindowHeight,0);
		ms.Scale(Application::GetInstance().GetWindowWidth(), Application::GetInstance().GetWindowHeight(), 1);
		RenderHelper::RenderMesh(MainMenu);
		ms.PopMatrix();

		ms.PushMatrix();
		ms.Translate(arrowPosition.x - halfWindowWidth, arrowPosition.y - halfWindowHeight, arrowPosition.z);
		ms.Scale(25.f, 25.f, 25.f);
		RenderHelper::RenderMesh(Arrow);
		ms.PopMatrix();
	}
}

void SceneText::RenderPassGPass()
{
	GraphicsManager* g = GraphicsManager::GetInstance();

	g->m_renderPass = g->RENDER_PASS_PRE;
	g->m_lightDepthFBO.BindForWriting();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(g->m_gPassShaderID);
	//These matrices should change when light position or direction changes
	Light* light = dynamic_cast<Light*>(g->GetLight("lights[0]"));
	if (light->type == Light::LIGHT_DIRECTIONAL) {
		g->m_lightDepthProj.SetToOrtho(-MAX_CELLS * CELL_SIZE / 1.5, MAX_CELLS * CELL_SIZE /  1.5, -MAX_CELLS * CELL_SIZE / 1.5, MAX_CELLS * CELL_SIZE / 1.5, -MAX_CELLS * CELL_SIZE * 2, MAX_CELLS * CELL_SIZE *  4);
		//g->m_lightDepthProj.SetToOrtho(-100, 100, -100, 100, -100, 100);
	}
	else
		g->m_lightDepthProj.SetToPerspective(90, 1.f, 0.1, 10);


	//Vector3 up = Vector3(-1, 0, 0).Cross(Vector3(-lights[0]->position.x, -lights[0]->position.y, -lights[0]->position.z)).Normalized();

	////g->m_lightDepthView.SetToLookAt(lights[0]->position.x,
	////	lights[0]->position.y, lights[0]->position.z, 0, 0, 0, up.x, up.y, up.z);

	//g->m_lightDepthView.SetToLookAt(lights[0]->position.x,
	//	lights[0]->position.y, lights[0]->position.z, MAX_CELLS * CELL_SIZE / 2, 0, MAX_CELLS * CELL_SIZE / 2, up.x, up.y, up.z);

	//std::cout << light->position.x << ", " << light->position.y << ", " << light->position.z << std::endl;

	Vector3 temp(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z);
	Mtx44 rotate;
	rotate.SetToRotation(-90, 1, 0, 0);
	temp = rotate * temp;

	Vector3 up = Vector3(-1, 0, 0).Cross(Vector3(-temp.x, -temp.y, -temp.z)).Normalized();

	g->m_lightDepthView.SetToLookAt(temp.x,
		temp.y, temp.z, MAX_CELLS * CELL_SIZE / 2, 0, MAX_CELLS * CELL_SIZE / 2, up.x, up.y, up.z);

	RenderWorld();
}
//******************************* MAIN RENDER PASS ************************************
void SceneText::RenderPassMain()
{
	GraphicsManager* g = GraphicsManager::GetInstance();
	MS& ms = GraphicsManager::GetInstance()->GetModelStack();

	g->m_renderPass = g->RENDER_PASS_MAIN;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Application::GetInstance().GetWindowWidth(),
		Application::GetInstance().GetWindowHeight());
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(g->GetActiveShader()->GetProgramID());
	//pass light depth texture
	g->m_lightDepthFBO.BindForReading(GL_TEXTURE8);
	glUniform1i(g->GetActiveShader()->GetUniform("shadowMap"), 8);
	//... old stuffs

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	GraphicsManager::GetInstance()->SetPerspectiveProjection(60.f, (float)(Application::GetInstance().m_window_width) / (float)(Application::GetInstance().m_window_height), 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(camera);

	ms.LoadIdentity();

	Light* light = dynamic_cast<Light*>(g->GetLight("lights[0]"));

	//placed down so alpha will work properly on ldq.
	ms.PushMatrix();
	//attempt to fix shadow issue
	ms.Translate(0, 0, 0);
	RenderWorld();
	ms.PopMatrix();

	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;


	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);

	ms.PushMatrix();
	ms.Translate(0.f, (float)halfWindowHeight * 0.775f, 0.f);
	ms.Rotate(180.f + 135.f, 0, 0, -1);
	ms.Rotate((calendarTime - 180.f), 0, 0, -1);
	ms.Scale(130.f, 130.f, 0.f);
	RenderHelper::RenderMesh(MeshList::GetInstance()->GetMesh("sunMoon"));
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate(0.f, (float)halfWindowHeight * 0.865f, 0.f);
	ms.Scale(200.f, 100.f, 0.f);
	RenderHelper::RenderMesh(MeshList::GetInstance()->GetMesh("calendar"));
	ms.PopMatrix();

	EntityManager::GetInstance()->RenderUI();
	theMiniMap->Init(halfWindowHeight, halfWindowWidth);
	theMiniMap->RenderUI();

	ms.PushMatrix();
	ms.Translate(0, (float)halfWindowHeight * 0.98f, 0);
	ms.Scale(core->GetHealth(), 10.f, 0);
	RenderHelper::RenderMesh(generatorCoreHealthBar);
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate((float)-halfWindowWidth, (float)halfWindowHeight * 0.92f, 0.f);
	ms.Scale(Player::GetInstance()->GetPlayerHealth(), 10.f, 0.f);
	RenderHelper::RenderMesh(playerHealthBar);
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate((float)-halfWindowWidth + 50.f, (float)halfWindowHeight * 0.82f, 0.f);
	ms.Scale(50.f, 50.f, 0.f);
	RenderHelper::RenderMesh(MeshList::GetInstance()->GetMesh("Fatigue"));
	ms.PopMatrix();

	if (Render_Quad && Player::GetInstance()->tab == 1)
	{
		ms.PushMatrix();
		//ms.Translate((float)-halfWindowWidth, (float)halfWindowHeight * 0.92f, 0.f);
		ms.Translate(0.f, 0.f, 0.f);
		ms.Scale((float)Application::GetInstance().GetWindowWidth() * 0.75f, (float)Application::GetInstance().GetWindowHeight() * 0.75f, 0.f);
		RenderHelper::RenderMesh(Shop);
		ms.PopMatrix();
	}
	else if (Render_Quad && Player::GetInstance()->tab == 2)
	{
		ms.PushMatrix();
		//ms.Translate((float)-halfWindowWidth, (float)halfWindowHeight * 0.92f, 0.f);
		ms.Translate(0.f, 0.f, 0.f);
		ms.Scale((float)Application::GetInstance().GetWindowWidth() * 0.75f, (float)Application::GetInstance().GetWindowHeight() * 0.75f, 0.f);
		RenderHelper::RenderMesh(Shop2);
		ms.PopMatrix();
	}

	//Building,Equipment,Weapon UI
	ms.PushMatrix();
	ms.Translate((float)halfWindowWidth * 0.88f, (float)halfWindowHeight * -0.78f, 0);
	ms.Translate(0.f, 0.f, 0.f);
	ms.Scale((float)Application::GetInstance().GetWindowWidth() * 0.08f, (float)Application::GetInstance().GetWindowHeight() * 0.06f, 0.f);
	RenderHelper::RenderMesh(BEW_UI);
	ms.PopMatrix();
	
	if (Player::GetInstance()->Render_Another_qUAD)
	{
		ms.PushMatrix();
		ms.Scale((float)Application::GetInstance().GetWindowWidth() * 0.75f, (float)Application::GetInstance().GetWindowHeight() * 0.75f, 0.f);
		RenderHelper::RenderMesh(Constrain);
		ms.PopMatrix();

	}

	for (int i = 0; i < Player::GetInstance()->interactionList.size(); ++i) {
		ms.PushMatrix();
		ms.Translate(Player::GetInstance()->interactionList[i]->GetPosition().x, Player::GetInstance()->interactionList[i]->GetPosition().y, Player::GetInstance()->interactionList[i]->GetPosition().z);
		ms.Scale(Player::GetInstance()->interactionList[i]->GetScale().x, Player::GetInstance()->interactionList[i]->GetScale().y, Player::GetInstance()->interactionList[i]->GetScale().z);
		//RenderHelper::RenderMesh(redquad);
		ms.PopMatrix();
	}

	EntityManager::GetInstance()->RenderUI();

	//RenderHelper::RenderTextOnScreen(text, std::to_string(fps), Color(0, 1, 0), 2, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
}

void SceneText::RenderWorld()
{
	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	ms.PushMatrix();
	ms.Translate(MAX_CELLS * CELL_SIZE / 2, 0, MAX_CELLS * CELL_SIZE / 2);
	ms.Rotate(-90, 1, 0, 0);
	ms.Scale(MAX_CELLS * CELL_SIZE, MAX_CELLS * CELL_SIZE, MAX_CELLS * CELL_SIZE);
	RenderHelper::RenderMeshWithLight(ground);
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate(MAX_CELLS * CELL_SIZE / 2, -0.1, MAX_CELLS * CELL_SIZE / 2);
	ms.Rotate(-90, 1, 0, 0);
	ms.Scale(MAX_CELLS * CELL_SIZE * 2, MAX_CELLS * CELL_SIZE * 2, MAX_CELLS * CELL_SIZE);
	RenderHelper::RenderMeshWithLight(background);
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate(Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().y + Player::GetInstance()->GetScale().y / 2, Player::GetInstance()->GetPosition().z);
	ms.Rotate(90, 0, 1, 0);
	ms.Rotate(-Math::RadianToDegree(atan2(Player::GetInstance()->direction.x, Player::GetInstance()->direction.y)), 0, 1, 0);
	ms.Scale(Player::GetInstance()->GetScale().x, Player::GetInstance()->GetScale().y, Player::GetInstance()->GetScale().z);
	RenderHelper::RenderMeshWithLight(Player::GetInstance()->GetMesh());
	ms.PopMatrix();

	if (isDay) // Only render wireframe box in day time
	{
		ms.PushMatrix();
		ms.Translate(ghostPos.x, ghostPos.y, ghostPos.z);
		ms.Rotate(-90, 1, 0, 0);
		ms.Scale(ghostScale.x, ghostScale.z, ghostScale.y);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderHelper::RenderMesh(wireFrameBox);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ms.PopMatrix();
	}

	
	EntityManager::GetInstance()->Render();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();

	BuildingManager::GetInstance()->Clear();
	EnemyManager::GetInstance()->ClearAll();
	EntityManager::GetInstance()->ClearEntities();
	// Delete the lights
	//delete lights[0];
}
