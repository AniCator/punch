#include "Vis_TestLayer1.h"

const int iPeriodPoints = 100;
const int iPeriodStep = FFT_DATAARRAY_SIZE / iPeriodPoints;

CVis_TestLayer1::CVis_TestLayer1()
{
	m_flOldRingSize = 0.3f;
	m_flRingSize = 0.3f;
	m_flCirclePeriod = 0.0f;

	m_flBaseRingSize = 0.3f;
	m_flBaseRingOffset = 0.3f;

	m_flBasePeriodMultiplier = 1.0f;

	m_iFrequency = 25;

	SimpleColor test;
	test.r = 1.0f;
	test.g = 0.0f;
	test.b = 0.5f;
	m_sColor = test;

	glGenBuffers(1, &VBO);
}

CVis_TestLayer1::CVis_TestLayer1(float flBaseRingSize, float flBaseRingOffset, float flBasePeriodMultiplier, int iFrequency, SimpleColor color)
{
	m_flOldRingSize = 0.3f;
	m_flRingSize = 0.3f;
	m_flCirclePeriod = 0.0f;
	
	m_flBaseRingSize = flBaseRingSize;
	m_flBaseRingOffset = flBaseRingOffset;
	m_flBasePeriodMultiplier = flBasePeriodMultiplier;

	m_iFrequency = iFrequency;

	m_flOldRingSize = flBaseRingSize;
	m_flRingSize = flBaseRingSize; 
	m_sColor = color;

	glGenBuffers(1, &VBO);
}

CVis_TestLayer1::~CVis_TestLayer1()
{

}

void CVis_TestLayer1::Think(FFT_DataArray fft_data)
{
	m_flCirclePeriod += fft_data.fft_data[m_iFrequency] * (50.0f * m_flBasePeriodMultiplier);
	m_flRingSize = m_flBaseRingOffset + fft_data.fft_data[m_iFrequency] * (50.0f * m_flBaseRingSize);
	m_flRingSize = (m_flRingSize * 0.01f) + (m_flOldRingSize*0.99f);
	m_flOldRingSize = m_flRingSize;

	m_flLastRecordedFrequencyValue = fft_data.fft_data[m_iFrequency];
	m_flAverageFrequencyValue += fft_data.fft_data[m_iFrequency];
	m_flAverageFrequencyValue /= 2.0f;

	points.clear();
	for(int i = 0; i < iPeriodPoints; i++)
	{
		float flPeriodMod = ((float)i + 1) / (float)iPeriodPoints;
		float flPeak = 1.0f + fft_data.fft_data[i * iPeriodStep] * 100.0f;

		float flPeriodMath = m_flCirclePeriod * flPeriodMod * 0.5f;
		float flSine = sin(flPeriodMath); 
		float flCosine = cos(flPeriodMath);

		for(int j = 0; j < 100; j++)
		{
			float flPeriodModStep = flPeriodMod / 100.0f;
			flPeriodModStep *= (float)j;
			flPeriodModStep += m_flRingSize;
			points.push_back(glm::vec3(flSine * m_flRingSize * flPeak * flPeriodModStep,flCosine * m_flRingSize * flPeak * flPeriodModStep ,0.0f));
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);
}

void CVis_TestLayer1::Draw()
{
	glPointSize(2.0f);

	float flNewValue = m_flLastRecordedFrequencyValue * 70.0f;
	float flColorMultiplier = 0.0f;
	flColorMultiplier = (flNewValue*0.1f) + (m_flOldColorMultiplier*0.9f);
	m_flOldColorMultiplier = flColorMultiplier;

	glColor3f(m_sColor.r * flColorMultiplier,m_sColor.g * flColorMultiplier,m_sColor.b * flColorMultiplier);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, points.size());
	glDisableVertexAttribArray(0);
}

void CVis_TestLayer1::Reset()
{
	m_flCirclePeriod = 0.0f;
}