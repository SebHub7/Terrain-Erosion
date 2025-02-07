#include <GL/glew.h>
#include <vector>
#include <vec.h>
#include <program.h>
#include "HeightField.h"
#include <orbiter.h>
#include <uniforms.h>


class TerrainBuffers
{
public:
	TerrainBuffers();
	void CreateBuffers(Image& img);
	void FreeBuffers();
	void UpdateBuffers(HeightField& hf);

	void AddVertexAndNormals(HeightField& hf);
	void draw(Orbiter& camera);

private:
	Image img;
	GLuint shader;
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;

	GLuint texture;

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;

	std::vector<unsigned int> indices;
};