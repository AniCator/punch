#include "Vis_TestLayer1.h"
#include "ConfigurationManager.h"
#include "util/util.h"
#include "appframework/PunchApplication.h"

#pragma optimize("", off)

VisualizationQuantumRing::VisualizationQuantumRing()
{
	OldRingSize = 0.0f;
	RingSize = 0.0f;
	CirclePeriod = 0.0f;

	BaseRingSize = 0.3f * (float) CConfigurationManager::GetInstance().GetDouble( "radial_size" );;
	BaseRingOffset = 0.3f;

	BasePeriodMultiplier = 1.0f;

	Frequency = 25;
	LastRecordedFrequencyValue = 0.0f;
	AverageFrequencyValue = 0.0f;

	FColor test;
	test.r = 1.0f;
	test.g = 0.0f;
	test.b = 0.5f;
	QuantumRingColor = test;

	PeriodPoints = 100;
	PeriodLines = 100;
	PeriodPointSize = 70.0f;
	PeriodStep = DataArraySizeFFT / PeriodPoints;
	PeriodBassFrequency = 1;
	PeriodBassMultiplier = 80.0f;

	InterpolationSpeed = glm::max( 0.0f, CConfigurationManager::GetInstance().GetFloat( "radial_interpolation_speed" ) );

	glGenBuffers( 1, &VBO );
}

VisualizationQuantumRing::VisualizationQuantumRing( float flBaseRingSize, float flBaseRingOffset, float flBasePeriodMultiplier, int iFrequency, FColor color )
{
	OldRingSize = 0.0f;
	RingSize = 0.0f;
	CirclePeriod = 100.0f;

	BaseRingSize = flBaseRingSize * CConfigurationManager::GetInstance().GetFloat( "radial_size" );;
	BaseRingOffset = flBaseRingOffset;
	BasePeriodMultiplier = flBasePeriodMultiplier;

	Frequency = iFrequency;
	LastRecordedFrequencyValue = 0.0f;
	AverageFrequencyValue = 0.0f;

	OldColorMultiplier = 0.0f;

	OldRingSize = flBaseRingSize;
	RingSize = flBaseRingSize;
	QuantumRingColor = color;

	PeriodPoints = CConfigurationManager::GetInstance().GetInteger( "radial_points" );
	PeriodLines = CConfigurationManager::GetInstance().GetInteger( "radial_lines" );
	PeriodPointSize = CConfigurationManager::GetInstance().GetFloat( "radial_point_size" );
	PeriodBassFrequency = CConfigurationManager::GetInstance().GetInteger( "radial_bass_frequency" );
	PeriodBassMultiplier = CConfigurationManager::GetInstance().GetFloat( "radial_bass_multiplier" );
	PeriodStep = DataArraySizeFFT / PeriodLines;

	InterpolationSpeed = glm::max( 0.0f, CConfigurationManager::GetInstance().GetFloat( "radial_interpolation_speed" ) );
	MedianMultiplier = glm::max( 0.0f, CConfigurationManager::GetInstance().GetFloat( "radial_median_multiplier" ) );

	for( unsigned int i = 0; i < DataArraySizeFFT; i++ )
	{
		DataPrevious.Data[i] = 0.0f;
	}

	glGenBuffers( 1, &VBO );
}

VisualizationQuantumRing::~VisualizationQuantumRing()
{

}

inline float GetInterpolatedFrequencyValue( const int Index, const float* CurrentDataSet, float* OldDataSet, const float InterpolationSpeed )
{
	const float DataNew = CurrentDataSet[Index];
	const float DataOld = OldDataSet[Index];
	const float Output = Lerp( DataOld, DataNew, InterpolationSpeed );
	OldDataSet[Index] = DataNew;

	return Output;
}

void VisualizationQuantumRing::Think( DataArrayFFT fft_data )
{
	CPunchApplication& PunchInstance = CPunchApplication::GetInstance();
	const float DeltaTime = static_cast<float>( PunchInstance.GetDeltaTime() );
	const float EaseSpeed = glm::clamp( InterpolationSpeed * DeltaTime, 0.0f, 1.0f );

	float DataArray[DataArraySizeFFT];
	std::vector<float> SortedDataArray;
	SortedDataArray.reserve( DataArraySizeFFT );

	for( unsigned int i = 0; i < DataArraySizeFFT; i++ )
	{
		DataArray[i] = GetInterpolatedFrequencyValue( i, fft_data.Data, DataPrevious.Data, EaseSpeed );
		SortedDataArray.push_back( DataArray[i] );
	}

	std::sort( SortedDataArray.begin(), SortedDataArray.end() );

	const int MedianIndex = SortedDataArray.size() / 2 + 1;
	const float MedianFrequencyValue = SortedDataArray[MedianIndex] * MedianMultiplier;
	const float MaximumFrequencyValue = SortedDataArray[SortedDataArray.size() - 1];

	const float FrequencyValue = DataArray[Frequency];

	float NewValue = LastRecordedFrequencyValue * PeriodPointSize;
	ColorMultiplier = Lerp( OldColorMultiplier, NewValue, EaseSpeed );
	OldColorMultiplier = ColorMultiplier;

	CirclePeriod += FrequencyValue * ( 50.0f * BasePeriodMultiplier );
	RingSize = BaseRingOffset + FrequencyValue * ( 50.0f * BaseRingSize );
	RingSize = ( RingSize * 0.01f ) + ( OldRingSize*0.99f );
	OldRingSize = RingSize;

	LastRecordedFrequencyValue = FrequencyValue;
	AverageFrequencyValue += FrequencyValue;
	AverageFrequencyValue *= 0.5f;

	points.clear();
	for( unsigned int i = 0; i < PeriodLines; i++ )
	{
		const float PeakValue = DataArray[i * PeriodStep];

		float flPeriodMod = ( (float) i + 1 ) / (float) PeriodLines;
		float flPeak = 1.0f + PeakValue * 100.0f;

		float flPeriodMath = CirclePeriod * flPeriodMod * 0.5f;

		float flSine = sin( flPeriodMath );
		float flCosine = cos( flPeriodMath );

		for( unsigned int j = 0; j < PeriodPoints; j++ )
		{
			float flPeriodModStep = flPeriodMod / PeriodPoints;
			flPeriodModStep *= (float) j;
			flPeriodModStep += RingSize;

			points.push_back( glm::vec3( flSine * RingSize * flPeak * flPeriodModStep, flCosine * RingSize * flPeak * flPeriodModStep, 0.0f ) );
		}
	}

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, points.size() * sizeof( glm::vec3 ), &points[0], GL_DYNAMIC_DRAW );
}

void VisualizationQuantumRing::Draw()
{
	const float PointSize = clamp( 1.0f + ( 2.5f * ColorMultiplier ), 0.0f, 200.0f );
	glPointSize( PointSize );

	float RingColor[3] = { QuantumRingColor.r, QuantumRingColor.g, QuantumRingColor.b };
	// RingColor[0] = Lerp( m_sColor.r, m_sColor.b, sin( (float) TimeKeeper::GetInstance().GetTime() * 0.03f ) * 0.5f + 0.5f );
	// RingColor[1] = Lerp( m_sColor.g, m_sColor.r, sin( (float) TimeKeeper::GetInstance().GetTime() * 0.025f ) * 0.5f + 0.5f );
	// RingColor[1] = Lerp( RingColor[1], m_sColor.r, cos( (float) TimeKeeper::GetInstance().GetTime() * 0.020f ) * 0.5f + 0.5f );
	// RingColor[2] = Lerp( m_sColor.b, m_sColor.r, cos( (float) TimeKeeper::GetInstance().GetTime() * 0.023f ) * 0.5f + 0.5f );

	unsigned int LeastIntenseColorIndex = 0;

	if( RingColor[1] < RingColor[0] )
		LeastIntenseColorIndex = 1;
	else if( RingColor[2] < RingColor[0] )
		LeastIntenseColorIndex = 2;

	// Multiply out color with least intensity to prevent having too much white on screen
	// const float Treshold = 0.5f;
	// if( RingColor[0] < Treshold && RingColor[1] < Treshold && RingColor[2] < Treshold )
	// 	RingColor[LeastIntenseColorIndex] *= 0.25f;

	/*RingColor[0] += ( abs( sin( TimeKeeper::GetInstance().GetTime() * 0.05f  ) ) * 0.4f );
	RingColor[1] += ( abs( cos( TimeKeeper::GetInstance().GetTime() * 0.025f ) ) * 0.4f );
	RingColor[2] += ( abs( tan( TimeKeeper::GetInstance().GetTime() * 0.075f ) ) * 0.4f );

	RingColor[0] = clamp( RingColor[0], 0.0f, 1.0f );
	RingColor[1] = clamp( RingColor[1], 0.0f, 1.0f );
	RingColor[2] = clamp( RingColor[2], 0.0f, 1.0f );*/

	glColor4f( RingColor[0], RingColor[1], RingColor[2], 1.0f / PeriodPoints );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_POINTS, 0, points.size() );
	glDisableVertexAttribArray( 0 );
}

void VisualizationQuantumRing::Reset()
{
	CirclePeriod = 0.0f;
}