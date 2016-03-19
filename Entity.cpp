#include "Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Entity::Entity(Mesh* p_mesh, Shape p_shapeType,
	glm::vec3 p_translation, glm::quat p_orientation, glm::vec3 p_scale, 
	glm::vec4 p_color, float p_mass) 
	: mesh(p_mesh), color(p_color), trueColor(p_color), 
	mass(p_mass)
{
	//http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Shapes
	btCollisionShape* shape;
	shapeType = p_shapeType;
	
	if(p_shapeType == Shape::Box)
		shape = new btBoxShape(btVector3(1,1,1));
	else 
		shape = new btCylinderShape(btVector3(1, 1, 1));

	shape->setLocalScaling(convert(p_scale));

	btDefaultMotionState* transform = 
		new btDefaultMotionState(btTransform(
							 btQuaternion(convert(p_orientation)), 
							 btVector3(convert(p_translation))));

	btVector3 inertia; 
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo 
		groundRigidBodyCI(mass, transform, shape, inertia);
	rigidbody = new btRigidBody(groundRigidBodyCI);

	rigidbody->setRestitution(0.0);

	const char* name = shape->getName();

	if(mass != 0)
		rigidbody->setActivationState(DISABLE_DEACTIVATION);
	//rigidbody->setMassProps(mass, inertia);

	rigidbody->setUserPointer(this);
}

Entity::~Entity()
{
	delete rigidbody;
	delete mesh;
}

glm::mat4 Entity::GetModelMatrix()
{
	return glm::translate(glm::mat4(1), GetTranslation())
		* glm::toMat4(GetOrientation())
		* glm::scale(glm::mat4(1), GetScale());
}

void Entity::Render()
{
	mesh->Draw();
}