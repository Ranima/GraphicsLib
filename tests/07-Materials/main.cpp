#include "graphics\Context.h"
#include "graphics\draw.h"
#include "graphics\Load.h"
#include "graphics\RenderObjects.h"
#include "graphics\Vertex.h"
#include "glm\ext.hpp"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	ofstream clearfile;
	clearfile.open("Test.txt", ios::out);
	clearfile.flush();
	clearfile.close();
	fstream thisFile;
	thisFile.open("Test.txt");

	Context context;
	context.init();

	Vertex vquad[] = {
		{ { -1,-1,0,1 },{},{ 0,0 },{ 0,0,1,0 } },
		{ {  1,-1,0,1 },{},{ 1,0 },{ 0,0,1,0 } },
		{ {  1, 1,0,1 },{},{ 1,1 },{ 0,0,1,0 } },
		{ { -1, 1,0,1 },{},{ 0,1 },{ 0,0,1,0 } }
	};

	unsigned quadidx[] = { 0,1,3, 1,2,3 };
	solveTangents(vquad, 4, quadidx, 6);
	Geometry quad = makeGeometry(vquad, 4, quadidx, 6);

	Geometry ss_geo = loadGeometry("../../resources/models/soulspear.obj");

	thisFile << "../../resources/textures/cone.jpg\n";
	thisFile << "../../resources/textures/lesserdog.jpg\n";
	thisFile << "../../resources/textures/jelly.jpg\n";

	string Fbuffer;

	int i = 0;
	Texture floor_normal[3];
	while (getline(thisFile, Fbuffer) && i <= 3)
	{
		floor_normal[i] = loadTexture()
	};
	//Texture floor_diffuse = loadTexture("../../resources/textures/soulspear_diffuse.tga");

	Shader standard = loadShader("../../resources/shaders/standard.vert",
								 "../../resources/shaders/standard.frag");

	Framebuffer screen = {0, 800, 600};

	// Camera
	glm::mat4 cam_view = glm::lookAt(glm::vec3(0, 2, 3),
									 glm::vec3(0, 0,  0),
									 glm::vec3(0, 1,  0));
	glm::mat4 cam_proj = glm::perspective(45.f, 800.f / 600.f, .01f, 100.f);
	
	// Model1
	glm::mat4 go_model1;

	// Light
	glm::vec3 light_direction = glm::normalize(glm::vec3(0,-1,-1));

	while (context.step())
	{
		float time = context.getTime();

		go_model1 = glm::rotate(time * 1, glm::vec3(0, 1, 0))
			* glm::rotate(glm::radians(0.f),
				glm::vec3(1, 0, 0))
			* glm::scale(glm::vec3(1, 1, 1)),
				glm::translate(glm::vec3());

		//go_model = glm::rotate(time, glm::vec3(0, 1, 0));

		clearFramebuffer(screen);
		setFlags(RenderFlag::DEPTH);

		int loc=0, slot=0;
		setUniforms(standard, loc, slot,
							cam_proj, cam_view,						// Camera information
							go_model1, floor_normal[0], //floor_diffuse,	// Surface information
							light_direction);						// Light Information
		
		s0_draw(screen, standard, quad);
	}
	thisFile.close();
	context.term();
	return 0;
}