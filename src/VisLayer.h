#pragma once
#include "appframework/errordefs.h"
#include "musicmodule/MusicModule.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h" //Include GLFW3 header

#include "glm.hpp"

struct FColor
{
	FColor() {};

	FColor(float rIn, float gIn, float bIn)
	{
		r = rIn;
		g = gIn;
		b = bIn;
	}

	float r;
	float g;
	float b;
};

class VisualizationLayer
{
public:
	VisualizationLayer();
	~VisualizationLayer();

	virtual void Think(DataArrayFFT fft_data) = 0;
	virtual void Draw() = 0;

	virtual void Reset() {};
};