#pragma once
#include "VisLayer.h"

class VisualizationQuantumRing : public VisualizationLayer
{
public:
	VisualizationQuantumRing();
	VisualizationQuantumRing(float flBaseRingSize, float flBaseRingOffset, float flBasePeriodMultiplier, int iFrequency, FColor color);
	~VisualizationQuantumRing();

	virtual void Think(DataArrayFFT fft_data);
	virtual void Draw();

	virtual void Reset();

	GLuint VBO;					//temp
	std::vector<glm::vec3> points;	//temp

	float BaseRingSize;
	float BaseRingOffset;
	float BasePeriodMultiplier;

	FColor QuantumRingColor;

	float RingSize;
	float OldRingSize;
	float CirclePeriod;
	
	int Frequency;
	float LastRecordedFrequencyValue;
	float AverageFrequencyValue;

	float ColorMultiplier;
	float OldColorMultiplier;

	unsigned int PeriodPoints;
	unsigned int PeriodLines;
	unsigned int PeriodStep;
	unsigned int PeriodBassFrequency;
	float PeriodBassMultiplier;
	float PeriodPointSize;

	DataArrayFFT DataPrevious;
	float InterpolationSpeed;

	float MedianMultiplier;
};