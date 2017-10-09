

#include "graphics\Context.h"
#include "graphics\RenderObjects.h"
#include "graphics\Vertex.h"
#include "graphics\draw.h"
#include "graphics\load.h"

#include "glm\ext.hpp"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

class Object
{
public:
	Geometry model;
	Texture texture;
};

int main()
{
	int i = 0;
	int lines = 0;

	// Create our window and rendering context
	Context context;
	context.init(800, 800);

	fstream file;
	file.open("Test.txt", ios::in);
	string line;
	Object Cube;

	while (getline(file, line))
	{
		if (line[0] != '@') {continue;}

		getline(file, line);
		Cube.model = loadGeometry(line.c_str());

		getline(file, line);
		Cube.texture = loadTexture(line.c_str());
		break;
	}



	Vertex vquad[] = {
		{ { -1,-1,0,1 },{},{ 0,0 } },
		{ { 1,-1,0,1 },{},{ 1,0 } },
		{ { 1, 1,0,1 },{},{ 1,1 } },
		{ { -1,1,0,1 },{},{ 0,1 } }
	};
	unsigned quadidx[] = { 0,1,3, 1,2,3 };
	Geometry quad = makeGeometry(vquad, 4, quadidx, 6);
	Shader sq = loadShader("../../resources/shaders/test.vert",
						   "../../resources/shaders/test.frag");
	Shader scube = loadShader("../../resources/shaders/cube.vert",
							  "../../resources/shaders/cube.frag");

	Framebuffer screen = {0, 800, 800};

	float x = 0, y = 0;
	while (context.step())
	{
		clearFramebuffer(screen);
		float time = context.getTime();

		int frame = 3;
		frame += context.getKey('W') * 3;
		frame += context.getKey('A') * 2;
		frame += context.getKey('D') * 1;
		frame %= 4;

		x += context.getKey('D') * .016;
		y += context.getKey('W') * .016;
		x -= context.getKey('A') * .016;
		y -= context.getKey('S') * .016;

		int loc = 0, tslot = 0;
		setUniforms(sq, loc, tslot, Cube.texture, (int)(time*3) % 4 + frame*4, 4, 4,x,y);	
		// s0_draw(screen, sq, quad);


		glm::mat4 mod_cube = glm::rotate(time, glm::vec3(1,1,1));

		setFlags(RenderFlag::DEPTH);

		loc = 0, tslot = 0;
		setUniforms(scube, loc, tslot, mod_cube, Cube.texture);
		s0_draw(screen, scube, Cube.model);
	}


	context.term();
	return 0;
}