
#include "graphics\Load.h"
#include "graphics\RenderObjects.h"

#define STB_IMAGE_IMPLEMENTATION
#include "STB\stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "OBJ\tiny_obj_loader.h"

#include <string>
#include <fstream>
#include <streambuf>

#include "graphics\Vertex.h"

Texture loadTexture(const char *path)
{
	Texture retval = { 0 };

	int w, h, c;
	unsigned char *pixels;

	stbi_set_flip_vertically_on_load(true);

	pixels = stbi_load(path, &w, &h, &c, STBI_default);

	retval = makeTexture(w, h, c, pixels);

	stbi_image_free(pixels);

	return retval;
}

std::string fileToString(const char *path)
{
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());

	return str;
}

Shader loadShader(const char *vpath, const char *fpath)
{
	Shader retval = { 0 };

	// load in all of the text as a string/character array
	// for each shader

	std::string vsource = fileToString(vpath);
	std::string fsource = fileToString(fpath);

	retval = makeShader(vsource.c_str(), fsource.c_str());

	return retval;
}

#include <random>
glm::vec4 randColor()
{
	return  {
		rand() / (float)RAND_MAX,
		rand() / (float)RAND_MAX,
		rand() / (float)RAND_MAX, 1}; }

Geometry loadGeometry(const char *path)
{
	Geometry retval = {0,0,0,0};
	tinyobj::attrib_t attrib;				// Vertex Data is stored.
	std::vector<tinyobj::shape_t> shapes;	// Triangular data, indices.
	std::vector<tinyobj::material_t> materials;
	std::string err;

	tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path);

	/////////////////////////////////////////////////////////////////
	size_t isize = shapes[0].mesh.indices.size();
	size_t *indices = new unsigned[isize];

	size_t vsize = isize;
	Vertex *verts = new Vertex[vsize];

	for (int i = 0; i < isize; ++i)
	{
		indices[i] = i;
		
		int pi = shapes[0].mesh.indices[i].vertex_index;
		int ni = shapes[0].mesh.indices[i].normal_index;
		int ti = shapes[0].mesh.indices[i].texcoord_index;

		const float *p = &attrib.vertices[pi*3];  // 3x
		const float *n = &attrib.normals[ni*3];   // 3x
		const float *t = &attrib.texcoords[ti*2]; // 2x

		verts[i].position = {p[0],p[1],p[2],1};
		verts[i].texCoord = {t[0],t[1]};
		verts[i].normal   = {n[0],n[1],n[2],0};
	}
	///////////////////////////////////////////////////////////////////

	solveTangents(verts, vsize, indices, isize);

	retval = makeGeometry(verts, vsize, indices, isize);

	delete[] verts;
	delete[] indices;
	return retval;
}

/*
non-interleaved:

[ppppp][nnnnn][ttttt]

[pnt][pnt][pnt][pnt][pnt]
*/