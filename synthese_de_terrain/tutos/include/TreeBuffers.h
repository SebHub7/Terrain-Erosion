#pragma once
#include <GL/glew.h>
#include <vector>
#include <vec.h>
#include <program.h>
#include <orbiter.h>
#include <uniforms.h>
#include <mesh.h>


class TreeBuffers
{
public:
	TreeBuffers();
	void CreateBuffers();
	void FreeBuffers();

	void draw(Orbiter& camera);

	std::vector<vec3> translations;

private:
	GLuint shader;
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint vbo_translations;

	GLuint fir_texture;
	Mesh obj;
};