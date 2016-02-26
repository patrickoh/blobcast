#include "Level.h"
#include <json.hpp>
#include <fstream>

Level::~Level()
{
	for (RigidBody *r : Objects)
		delete r;
}

std::size_t Level::AddBox(
		glm::vec3 position,
		glm::quat orientation,
		glm::vec3 dimensions,
		glm::vec4 color)
{
	Mesh *box(Mesh::CreateCubeWithNormals(new VertexArray()));
	RigidBody *r =
		new RigidBody(box, position, orientation, 2.f * dimensions, color, 0);
	Objects.push_back(r);
	return Objects.size() - 1;
}

void Level::Delete(std::size_t index)
{
	Objects.erase(Objects.begin() + index);
}

int Level::Find(btRigidBody *r)
{
	for (std::size_t i = 0, n = Objects.size(); i < n; i++)
	{
		if (Objects[i]->rigidbody == r)
			return i;
	}
	return -1;
}

void Level::Render(GLuint uMMatrix, GLuint uColor)
{
	for (RigidBody *r : Objects)
	{
		glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, &r->GetModelMatrix()[0][0]);
		glUniform4fv(uColor, 1, &r->color.r);
		r->Render();
	}
}

void Level::Serialize(std::string file)
{
	std::ofstream f(file);
	nlohmann::json objects;
	for (RigidBody *r : Objects)
	{
		nlohmann::json object;
		object["type"] = "box";
		object["position"] = {
			r->translation.x, r->translation.y, r->translation.z };
		object["orientation"] = {
			r->orientation.w,
			r->orientation.x,
			r->orientation.y,
			r->orientation.z };
		object["dimensions"] = {
			r->scale.x, r->scale.y, r->scale.z };
		object["color"] = {
			r->color.r, r->color.g, r->color.b, r->color.a };
		objects.push_back(object);
	}
	nlohmann::json level;
	level["objects"] = objects;
	f << std::setw(2) << level << std::endl;
	f.close();
}

Level *Level::Deserialize(std::string file)
{
	std::ifstream f(file);
	if (!f.is_open())
		return NULL;
	std::string s(
			(std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
	auto data = nlohmann::json::parse(s);
	Level *level = new Level();
	for (auto object : data["objects"])
	{
		auto j_pos = object["position"];
		glm::vec3 position(j_pos[0], j_pos[1], j_pos[2]);
		auto j_ori = object["orientation"];
		glm::quat orientation(j_ori[0], j_ori[1], j_ori[2], j_ori[3]);
		auto j_dim = object["dimensions"];
		glm::vec3 dimensions(j_dim[0], j_dim[1], j_dim[2]);
		auto j_col = object["color"];
		glm::vec4 color(j_col[0], j_col[1], j_col[2], j_col[3]);
		level->AddBox(position, orientation, dimensions, color);
	}
	f.close();
	return level;
}