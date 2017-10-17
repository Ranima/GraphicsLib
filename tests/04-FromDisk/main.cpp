

#include "graphics\Context.h"
#include "graphics\RenderObjects.h"
#include "graphics\Vertex.h"
#include "graphics\draw.h"
#include "graphics\load.h"

#include "glm\ext.hpp"

#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

class Object
{
public:
	string geoName = "";
	string textureName = "";
};

struct Assets
{
private:
	Assets() {}
public:
	~Assets() {}

	std::vector<Object> objects;

	//std::map<std::string, Geometry> geos;		// name to geo
	//std::map<std::string, Texture> textures;	// name to tex


	std::map<std::string, std::string> geoPath; // name to geoPath
	std::map<std::string, std::string> texPath; // name to texPath

	std::map<std::string, Geometry> geoAssets; // geoPath to geo
	std::map<std::string, Texture> textureAssets; // texPath to tex

	bool hasLoadedGeo(std::string assetPath)
	{
		return geoAssets.count(assetPath);
	}

	bool hasLoadedTex(std::string assetPath)
	{
		return textureAssets.count(assetPath);
	}

	Geometry getGeoByName(std::string name)
	{
		Geometry ret = geoAssets[geoPath[name]];
		return ret;
	}

	Texture getTexByName(std::string name)
	{
		Texture ret = textureAssets[texPath[name]];
		return ret;
	}


	static Assets& getInstance()
	{
		static Assets * instance = new Assets();
		return *instance;
	}

	bool loadScene(std::string scenePath)
	{
		fstream file;
		file.open(scenePath, ios::in);
		
		std::string holder;
		string line;

		bool loaded = false;

		while (getline(file, line))
		{
			if (line[0] != '@') { continue; }
			loaded = true;

			// name for geo
			getline(file, line);
			holder = line.c_str();

			// path to geometry
			Object newObject;
			getline(file, line);

			// load asset if first encounter
			if (!hasLoadedGeo(line))
			{
				geoAssets[line] = loadGeometry(line.c_str());
			}

			geoPath[holder] = line.c_str();
			newObject.geoName = holder;
			
			// name for tex
			getline(file, line);
			holder = line.c_str();

			// path to tex
			getline(file, line);

			if (!hasLoadedTex(line))
			{
				textureAssets[line] = loadTexture(line.c_str());
			}
			texPath[holder] = line.c_str();
			newObject.textureName = holder;

			objects.push_back(newObject);
		}

		file.close();

		return loaded;
	}
};

int main()
{
	// Create our window and rendering context
	Context context;
	context.init(800, 800);

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

	Assets &assets = Assets::getInstance();
	assets.loadScene("Test.txt");

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

		for (int i = 0; i < assets.objects.size(); ++i)
		{
			int loc = 0, tslot = 0;
			setUniforms(sq, loc, tslot, assets.getTexByName(assets.objects[i].textureName), (int)(time*3) % 4 + frame*4, 4, 4,x,y);
			s0_draw(screen, sq, quad);

			glm::mat4 mod_cube = glm::rotate(time, glm::vec3(1,1,1));

			setFlags(RenderFlag::DEPTH);

			loc = 0, tslot = 0;
			setUniforms(scube, loc, tslot, mod_cube, assets.getTexByName(assets.objects[i].textureName));
			s0_draw(screen, scube, assets.getGeoByName(assets.objects[i].geoName));
		}
	}


	context.term();
	return 0;
}