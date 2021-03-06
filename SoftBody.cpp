#include "SoftBody.h"

SoftBody::SoftBody(btSoftBody* p_softBody)
{
	softbody = p_softBody;

	vertices = std::vector<glm::vec3>(softbody->m_nodes.size());
	normals = std::vector<glm::vec3>(softbody->m_nodes.size());
	for (int i = 0; i<softbody->m_faces.size(); ++i)
	{
		const btSoftBody::Face&	f = softbody->m_faces[i];
		const btSoftBody::Node *node = &softbody->m_nodes[0];
		indices.push_back(f.m_n[0] - node);
		indices.push_back(f.m_n[1] - node);
		indices.push_back(f.m_n[2] - node);
	}
	for (int i = 0, n = softbody->m_nodes.size(); i < n; i++)
	{
		vertices.push_back(convert(softbody->m_nodes[i].m_x));
		vertices[i] = convert(softbody->m_nodes[i].m_x);
		normals[i] = convert(softbody->m_nodes[i].m_n);
	}

	glGenVertexArrays(1, &vao);
	VBOs = new GLuint[2];
	glGenBuffers(2, VBOs);
	glGenBuffers(1, &IBO);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STREAM_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SoftBody::~SoftBody()
{
	glDeleteVertexArrays(1, &vao);
	delete softbody;
	delete[] VBOs;
}

void SoftBody::Update()
{
	vertices = std::vector<glm::vec3>(softbody->m_nodes.size());
	normals = std::vector<glm::vec3>(softbody->m_nodes.size());
#pragma loop(hint_parallel(0))
#pragma loop(ivdep)
	for (int i = 0, n = softbody->m_nodes.size(); i < n; i++)
	{
		vertices[i] = convert(softbody->m_nodes[i].m_x);
		normals[i] = convert(softbody->m_nodes[i].m_n);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SoftBody::Render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void SoftBody::RenderPatches()
{
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glBindVertexArray(vao);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}
