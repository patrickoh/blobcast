#include "Blob.h"

Blob::Blob(
		btSoftBodyWorldInfo& softBodyWorldInfo,
		const btVector3& center, btScalar r, int vertices) :
	SoftBody(btSoftBodyHelpers::CreateEllipsoid(
				softBodyWorldInfo, center, btVector3(1, 1, 1) * r, vertices)),
	centroid(center),
	radius(r),
	speed(1120.f / vertices)
{
	forward = btVector3(0, 0, 1);
	btVector3 scale = btVector3(1, 1, 1) * r;
	btVector3 points[6] = {
		btVector3(center[0], center[1] - scale[1], center[2]),
		btVector3(center[0], center[1] + scale[1], center[2]),
		btVector3(center[0] - scale[0], center[1], center[2]),
		btVector3(center[0] + scale[0], center[1], center[2]),
		btVector3(center[0], center[1], center[2] - scale[2]),
		btVector3(center[0], center[1], center[2] + scale[2]) };
	for (unsigned int i = 0, n = softbody->m_nodes.size(); i < n; i++)
	{
		btSoftBody::Node *node = &softbody->m_nodes[i];
		for (unsigned int j = 0; j < 6; j++)
		{
			if (sampleNodes[j] == NULL)
				sampleNodes[j] = node;
			else if (btDistance2(node->m_x, points[j]) <
					btDistance2(sampleNodes[j]->m_x, points[j]))
			{
				sampleNodes[j] = node;
			}
		}
	}

	softbody->m_materials[0]->m_kLST = 0.1;
	softbody->m_cfg.kDF = 1;
	softbody->m_cfg.kDP = 0.001;
	softbody->m_cfg.kPR = 2500;
	softbody->setTotalMass(30, true);
}

Blob::~Blob()
{

}

void Blob::Update()
{
	SoftBody::Update();
	ComputeCentroid();
}

void Blob::Move(int key, int action)
{
	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		AddForce(btVector3(0, 0, 1));
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		AddForce(btVector3(0, 0, -1));
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		AddForce(btVector3(1, 0, 0));
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		AddForce(btVector3(-1, 0, 0));
	if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
		AddForce(btVector3(0, 1, 0));
}

void Blob::AddForce(const btVector3 &force)
{
	if (force.length() > 0.0f)
		softbody->addForce(force * speed);
}

void Blob::AddForce(const btVector3 &force, int i)
{
	if (force.length() > 0.0f)
		softbody->addForce(force * speed, i);
}

void Blob::AddForces(const AggregateInput& inputs)
{
	float total = (float)inputs.TotalCount;
	if (total == 0.0f)
		total = 1.0f;
	float magFwd = (float)inputs.FCount / total,
		  magBack = (float)inputs.BCount / total,
		  magRight = (float)inputs.RCount / total,
		  magLeft = (float)inputs.LCount / total,
		  magFR = (float)inputs.FRCount / total,
		  magFL = (float)inputs.FLCount / total,
		  magBR = (float)inputs.BRCount / total,
		  magBL = (float)inputs.BLCount / total;
	btVector3 right = forward.cross(btVector3(0, 1, 0));
	btVector3 fwdright = (forward + right) * SIMDSQRT12;
	btVector3 fwdleft = (forward - right) * SIMDSQRT12;
#pragma loop(hint_parallel(0))
#pragma loop(ivdep)
	for (int i = 0, n = softbody->m_nodes.size(); i < n; i++)
	{
		btVector3 blobSpaceDir =
			((softbody->m_nodes[i].m_x - centroid) *
			 btVector3(1, 0, 1)).normalized();
		btScalar magnitude = btMin(
			btPow(btMax(btDot(blobSpaceDir, forward), 0.f), 2) * magFwd +
			btPow(btMax(btDot(blobSpaceDir, -forward), 0.f), 2) * magBack +
			btPow(btMax(btDot(blobSpaceDir, right), 0.f), 2) * magRight +
			btPow(btMax(btDot(blobSpaceDir, -right), 0.f), 2) * magLeft +
			btPow(btMax(btDot(blobSpaceDir, fwdright), 0.f), 2) * magFR +
			btPow(btMax(btDot(blobSpaceDir, fwdleft), 0.f), 2) * magFL +
			btPow(btMax(btDot(blobSpaceDir, -fwdleft), 0.f), 2) * magBR +
			btPow(btMax(btDot(blobSpaceDir, -fwdright), 0.f), 2) * magBL,
			1.0f);
		btVector3 force = blobSpaceDir * magnitude;
		AddForce(force, i);
	}

	AddForce(btVector3(0, 1, 0) * inputs.JCount / total);
}

void Blob::ComputeCentroid()
{
	centroid = btVector3(0, 0, 0);
	for (int i = 0; i < 6; i++)
		centroid += sampleNodes[i]->m_x;
	centroid *= (1.f/6.f);
}

btVector3 Blob::GetCentroid()
{
	return centroid;
}

void Blob::DrawGizmos(ShaderProgram* shaderProgram)
{
	glm::vec3 L = convert(forward.rotate(btVector3(0, 1, 0), -glm::quarter_pi<float>()));
	glm::vec3 R = convert(forward.rotate(btVector3(0, 1, 0), glm::quarter_pi<float>()));
	glm::vec3 BL = -R;
	glm::vec3 BR = -L;

	glm::vec3 c = convert(GetCentroid());

	Line l(c, c + L * 10.0f);
	Line r(c, c + R * 10.0f);
	Line bl(c, c + BL * 10.0f);
	Line br(c, c + BR * 10.0f);
	Line fwd(c, c + convert(forward) * 10.0f);

	shaderProgram->Use([&](){
		l.Render();
		r.Render();
		bl.Render();
		br.Render();
	});

	(*shaderProgram)["uColor"] = glm::vec4(1, 0, 0, 1);
	shaderProgram->Use([&](){
		fwd.Render();
	});
}
