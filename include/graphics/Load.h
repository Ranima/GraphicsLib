#pragma once

#include "RenderObjects.h"
using namespace std;

Texture loadTexture(const char *path);
Texture loadTexture(const fstream *path);

Shader loadShader(const char *vpath, const char *fpath);



Geometry loadGeometry(const char *path);