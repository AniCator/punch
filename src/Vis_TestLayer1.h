#pragma once
#include "VisLayer.h"

class CVis_TestLayer1 : public CVisLayer
{
public:
	CVis_TestLayer1();
	CVis_TestLayer1(float flBaseRingSize, float flBaseRingOffset, float flBasePeriodMultiplier, int iFrequency, SimpleColor color);
	~CVis_TestLayer1();

	virtual void Think(FFT_DataArray fft_data);
	virtual void Draw();

	virtual void Reset();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp

	float m_flBaseRingSize;
	float m_flBaseRingOffset;
	float m_flBasePeriodMultiplier;

	SimpleColor m_sColor;

	float m_flRingSize;
	float m_flOldRingSize;
	float m_flCirclePeriod;
	
	int m_iFrequency;
	float m_flLastRecordedFrequencyValue;
	float m_flAverageFrequencyValue;

	float m_flOldColorMultiplier;
};