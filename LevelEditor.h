#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include <imgui.h>
#include "imgui_impl_glfw.h"

#include "glm/glm.hpp"

#include <sstream>
#include <iostream>

#include "GameObject.h"

#include <typeinfo>
#include <set>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ShaderProgram.h"

#include "Level.h"
#include "Physics.h"
#include <stdio.h>
#include "tinyfiledialogs.h"

class LevelEditor
{

private:

public:

	bool bShowBlobCfg = false;
	bool bShowImguiDemo = false;
	bool bShowCameraSettings = true;

	std::set<GameObject*> selection;
	bool bLocal = true;
	bool bCtrl = false;
	bool bSetLink = false;

	LevelEditor(){}
	~LevelEditor(){}

	void MainMenuBar();
	void SelectionWindow(ShaderProgram *shaderProgram);
	void Mouse(double xcursor, double ycursor, int width, int height,
		glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	void DeleteSelection(); 
	void CloneSelection();
	void DrawPath(const ShaderProgram& program);

private:
	void Translation();
	void TranslateSelection(glm::vec3 translate);
	void Rotation(ShaderProgram *shaderProgram);
	void LocalRotation(float angle, glm::vec3 axis);
	void GlobalRotation(float angle, glm::vec3 axis, glm::vec3 axisPosition);
	void Scale();
	void ScaleSelection(glm::vec3 relScale);
	void Path();
	void NewSelection(GameObject* newSelection);
	void DrawRotationGizmo(glm::vec3 axis, glm::quat orientation,
		glm::vec3 translation, ShaderProgram *shaderProgram, glm::vec4 color);
	void ClearSelection();
};
