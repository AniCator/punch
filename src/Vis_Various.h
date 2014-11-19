#pragma once
#include "VisLayer.h"

class CVis_CubeWalking : public CVisLayer
{
public:
	CVis_CubeWalking( int iFrequency );
	~CVis_CubeWalking();

	virtual void Think(FFT_DataArray fft_data);
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