#pragma once
#include "VisLayer.h"

class VisualizationSpectrum : public VisualizationLayer
{
public:
	VisualizationSpectrum();
	~VisualizationSpectrum();

	virtual void Think(DataArrayFFT fft_data);
	virtual void Draw();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp
	DataArrayFFT m_DataOld;
	float InterpolationSpeed;
	
	int SpectrumBars;
	float SpectrumHeight;
};