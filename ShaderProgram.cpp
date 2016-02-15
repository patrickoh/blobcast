#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::ShaderProgram(std::vector<Shader *> shaders)
{
	program = glCreateProgram();
	LinkProgram(shaders);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(program);
}

void ShaderProgram::LinkProgram(std::vector<Shader *> &shaders)
{
	for (std::size_t i = 0; i < shaders.size(); i++)
	{
		glAttachShader(program, shaders[i]->Name);
	}
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		std::cerr << "Shader linking failed with this message:" << std::endl;
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;

		// TODO don't just terminate the application?
		exit(1);
	}

	for (std::size_t i = 0; i < shaders.size(); i++)
	{
		glDetachShader(program, shaders[i]->Name);
	}

	GLint num_uniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_uniforms);
	GLint uniform_name_length;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_name_length);
	GLchar *name = new GLchar[uniform_name_length];
	for (GLint i = 0; i < num_uniforms; i++)
	{
		GLint size;
		GLenum type;
		glGetActiveUniform(
				program, i, uniform_name_length, NULL, &size, &type, name);
		uniforms[name] = i;
	}
	delete[] name;
}

GLint ShaderProgram::GetUniformLocation(std::string name) const
{
	auto it = uniforms.find(name);
	if (it == uniforms.end())
	{
		return -1;
	}
	else
	{
		return it->second;
	}
}

void ShaderProgram::DrawFrame(Frame *frame, glm::mat4 mvMatrix) const
{
	Install();
	GLuint uMVMatrix = glGetUniformLocation(program, "uMVMatrix");
	GLuint uNMatrix = glGetUniformLocation (program, "uNMatrix");
	frame->Draw(uMVMatrix, uNMatrix, mvMatrix);
	Uninstall();
}

void ShaderProgram::Install() const
{
	glUseProgram(program);
}

void ShaderProgram::Uninstall() const
{
	glUseProgram(0);
}