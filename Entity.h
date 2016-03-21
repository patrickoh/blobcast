#pragma once

#include <btBulletDynamicsCommon.h>
#include "Mesh.h" 
#include "Path.h"
#include <glm/gtc/type_ptr.hpp>
#include "Helper.h"

#include <glm/gtc/matrix_transform.hpp> 

enum Shape { Box, Cylinder };

class Entity
{

private:

	Mesh* mesh;
	bool collidable;

public:

	btRigidBody* rigidbody;
	glm::vec4 color;
	glm::vec4 trueColor;
	float mass;
	Shape shapeType;
	static int nextID;
	int ID;
	GLuint textureID;

	Entity(Mesh* p_mesh, Shape p_shapeType, glm::vec3 p_translation,
		glm::quat p_orientation, glm::vec3 p_scale, glm::vec4 p_color, 
		GLuint p_texID, float p_mass = 0, bool bCollidable = true);
	~Entity();

	glm::mat4 GetModelMatrix();

	virtual void Render();
	
	virtual void Update(float deltaTime) = 0;

	glm::quat GetOrientation() 
	{
		return convert(rigidbody->getOrientation());
	}
	glm::vec3 GetTranslation() 
	{
		return convert(rigidbody->getWorldTransform().getOrigin());
	}
	glm::vec3 GetScale()
	{
		return convert(rigidbody->getCollisionShape()->getLocalScaling());
	}

	void SetCollidable(bool set)
	{
		collidable = set;
		if (collidable)
			rigidbody->setCollisionFlags(rigidbody->CF_STATIC_OBJECT);
		else
			rigidbody->setCollisionFlags(rigidbody->CF_NO_CONTACT_RESPONSE);
	}

	bool GetCollidable()
	{
		return collidable;
	}
};



