#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include "Buffer.h"
#include "Drawable.h"
#include <vector>

class Mesh : public Drawable
{
	public:
		VertexArray VAO;
		std::vector<FloatBuffer> VBOs;
		ElementBuffer IBO;

		int NumVerts;
		int NumTris;

		Mesh(int vertexBuffers, int numVerts, int numFaces);
		Mesh(
				std::vector<GLfloat> vbo,
				std::vector<GLfloat> nbo,
				std::vector<GLuint> ibo);
		void Draw() const;
		void SetVertexData(
				GLuint attribute, GLfloat *data, int itemSize);
		void SetIndexData(unsigned int *iboData);
		void ComputeAABB(
				float& min_x, float& min_y, float& min_z,
				float& max_x, float& max_y, float& max_z) const;
		float ComputeRadius(glm::vec3 center) const;
		static Mesh *CreateCube();
		static Mesh *CreateTriplePlane();
		static Mesh *CreateCubeWithNormals();
		static Mesh *CreateQuad();
		static Mesh *CreateCylinderWithNormals(int segments = 30);

		static void Mesh::computeTangentBasis(float* vbo, float* texCoords, std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents);
};
