#pragma once
#include "VisLayer.h"

class CVis_Spectrum : public CVisLayer
{
public:
	CVis_Spectrum();
	~CVis_Spectrum();

	virtual void Think(FFT_DataArray fft_data);
	virtual void Draw();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp
};