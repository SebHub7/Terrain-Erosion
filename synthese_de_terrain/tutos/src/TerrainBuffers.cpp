#include "tutos/include/TerrainBuffers.h"
#include <texture.h>

TerrainBuffers::TerrainBuffers() 
{ 
	vao = vbo_positions = vbo_normals = shader = 0;
	texture = 0;
	vbo_uvs = 0;
}

void TerrainBuffers::CreateBuffers(Image& img)
{
	this->img = img;

	glClearColor(0.0f, 0.3f, 0.5f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_positions);
	glGenBuffers(1, &vbo_normals);
	glGenBuffers(1, &vbo_uvs);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions.front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals.front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(1);

	// uvs
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs.front().x, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// texture
	texture = make_texture(0, img);

	// shader
	shader = read_program("tutos/terrain_shader.glsl");
	program_print_errors(shader);
}

void TerrainBuffers::FreeBuffers()
{
	release_program(shader);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &vbo_positions);
	glDeleteBuffers(1, &vbo_normals);
	glDeleteBuffers(1, &vbo_uvs);

	glDeleteTextures(0, &texture);
}

void TerrainBuffers::UpdateBuffers(HeightField& hf)
{
	positions.clear();
	normals.clear();

	for (size_t y = 0; y < hf.getNy(); y++)
		for (size_t x = 0; x < hf.getNx(); x++)
		{
			positions.push_back(hf.Vertex(x, y));
			normals.push_back(hf.Normal(x, y));
		}

	glBindVertexArray(vao);

	//positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions.front().x, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals.front().x, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
}



void TerrainBuffers::AddVertexAndNormals(HeightField& hf)
{
	// ajout des sommets, normales et uvs aux vectors
	for (size_t y = 0; y < hf.getNy(); y++)
		for (size_t x = 0; x < hf.getNx(); x++)
		{
			vec2 grad = hf.Gradient(x, y);

			positions.push_back(hf.Vertex(x, y));
			normals.push_back(hf.Normal(x, y));
			//normals.push_back(vec3(grad.x, 0, grad.y));
			uvs.push_back(vec2((float)x / hf.getNx(), (float)y / hf.getNy()));
		}

	// creation of indices for better performances
	for (size_t y = 0; y < hf.getNy() - 1; y++)
		for (size_t x = 0; x < hf.getNx() - 1; x++)
		{
			unsigned int index_vertex = y * hf.getNx() + x;
			unsigned int index_vertex2 = (y + 1) * hf.getNx() + x;

			// triangle 1
			indices.push_back(index_vertex);
			indices.push_back(index_vertex2);
			indices.push_back(index_vertex + 1);

			// triangle 2
			indices.push_back(index_vertex + 1);
			indices.push_back(index_vertex2);
			indices.push_back(index_vertex2 + 1);
		}
}

void TerrainBuffers::draw(Orbiter& camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(vao);

	Transform view = camera.view();
	Transform projection = camera.projection();
	Transform vp = projection * view;

	glDeleteTextures(1, &texture);
	texture = make_texture(0, img);

	program_uniform(shader, "vpMatrix", vp);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices.front());
}