#pragma once
#include "VisLayer.h"

class VisualizationSpectrum : public VisualizationLayer
{
public:
	VisualizationSpectrum();
	~VisualizationSpectrum();

	virtual void Think(FFTDataArray fft_data);
	virtual void Draw();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp
	FFTDataArray m_DataOld;
	float m_flInterpolationSpeed;
};