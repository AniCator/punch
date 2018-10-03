#pragma once
#include "VisLayer.h"

class VisualizationCubeWalking : public VisualizationLayer
{
public:
	VisualizationCubeWalking( int iFrequency );
	~VisualizationCubeWalking();

	virtual void Think(DataArrayFFT fft_data);
	virtual void Draw();
	virtual void Reset();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp

	int iProgressionX;
	int iProgressionY;
	int m_iFrequency;

	float flSizeDataOld;
	float flProgressionStepsXOld;
};