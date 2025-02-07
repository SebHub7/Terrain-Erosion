#include "tutos/include/TreeBuffers.h"
#include <texture.h>
#include <wavefront.h>

TreeBuffers::TreeBuffers()
{
	vao = vbo_positions = vbo_normals = shader = 0;
	fir_texture = 0;
	vbo_uvs = 0;
	vbo_translations = 0;
}

void TreeBuffers::CreateBuffers()
{
	obj = read_mesh("tutos/data/sapin/fir.obj");

	for (vec3& v : obj.positions2())
	{
		v.x /= 3;
		v.y /= 3;
		v.z /= 3;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_positions);
	glGenBuffers(1, &vbo_normals);
	glGenBuffers(1, &vbo_uvs);
	glGenBuffers(1, &vbo_translations);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, obj.vertex_buffer_size(), &obj.positions().front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, obj.normal_buffer_size(), &obj.normals().front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(1);

	// uvs
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, obj.texcoord_buffer_size(), &obj.texcoords().front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// for translations
	glBindBuffer(GL_ARRAY_BUFFER, vbo_translations);
	glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(vec3), &translations.front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	
	// shader
	shader = read_program("tutos/tree_shader.glsl");
	program_print_errors(shader);

	// texture
	fir_texture = read_texture(1, "tutos/data/sapin/branch.png");
	program_use_texture(shader, "fir_text", 1, fir_texture);
}

void TreeBuffers::FreeBuffers()
{
	release_program(shader);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &vbo_positions);
	glDeleteBuffers(1, &vbo_normals);
	glDeleteBuffers(1, &vbo_uvs);
	glDeleteBuffers(1, &vbo_translations);

	glDeleteTextures(0, &fir_texture);
	obj.release();
}


void TreeBuffers::draw(Orbiter& camera)
{
	glUseProgram(shader);
	glBindVertexArray(vao);

	Transform view = camera.view();
	Transform projection = camera.projection();
	Transform vp = projection * view;

	program_uniform(shader, "vpMatrix", vp);

	glDrawArraysInstanced(GL_TRIANGLES, 0, obj.vertex_count(), translations.size());
}