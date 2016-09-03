#pragma once
#include "appframework/errordefs.h"
#include "musicmodule/MusicModule.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h" //Include GLFW3 header

#include "glm.hpp"

struct SimpleColor
{
	SimpleColor() {};
	SimpleColor(float r1, float g1, float b1) { r=r1;g=g1;b=b1; }

	float r;
	float g;
	float b;
};

class VisualizationLayer
{
public:
	VisualizationLayer();
	~VisualizationLayer();

	virtual void Think(FFTDataArray fft_data) = 0;
	virtual void Draw() = 0;

	virtual void Reset() {};
};