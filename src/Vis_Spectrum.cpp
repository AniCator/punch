#include "Vis_Spectrum.h"

#include "util/util.h"
#include "ConfigurationManager.h"
#include "appframework/PunchApplication.h"

VisualizationSpectrum::VisualizationSpectrum()
{
	glGenBuffers( 1, &VBO );
	InterpolationSpeed = glm::max( 0.0f, CConfigurationManager::GetInstance().GetFloat( "spectrum_interpolation_speed" ) );
	SpectrumBars = CConfigurationManager::GetInstance().GetInteger( "spectrum_bars" );
	if( SpectrumBars < 0 )
	{
		SpectrumBars = DataArraySizeFFT;
	}

	SpectrumHeight = CConfigurationManager::GetInstance().GetFloat( "spectrum_height" );

	for( int i = 0; i < DataArraySizeFFT; i++ )
	{
		m_DataOld.Data[i] = 0.0f;
	}
}

VisualizationSpectrum::~VisualizationSpectrum()
{

}
void VisualizationSpectrum::Think( DataArrayFFT fft_data )
{
	CPunchApplication& PunchInstance = CPunchApplication::GetInstance();
	const float DeltaTime = static_cast<float>( PunchInstance.GetDeltaTime() );
	const float EaseSpeed = glm::clamp( InterpolationSpeed * DeltaTime, 0.0f, 1.0f );

	points.clear();
	for( int i = 0; i < SpectrumBars; i++ )
	{
		int SpectrumIndex = i * ( DataArraySizeFFT / SpectrumBars );

		float flDataNew = fft_data.Data[SpectrumIndex];
		float flDataOld = m_DataOld.Data[SpectrumIndex];
		flDataNew = Lerp( flDataOld, flDataNew, InterpolationSpeed * EaseSpeed );
		m_DataOld.Data[SpectrumIndex] = flDataNew;

		float flHeight = (float) flDataNew * SpectrumHeight;
		float flModifier = 1.0f;
		float flHalfWidth = ( 2.0f / (float) SpectrumBars ) * flModifier * 0.4f;
		float flX = ( ( 2.0f / (float) SpectrumBars ) * i * flModifier ) - 1.0f;
		float flY = flHeight * ( 2.0f * flModifier ) - 1.0f;

		points.push_back( glm::vec3( flX - flHalfWidth, -1.0f, 0.0f ) );
		points.push_back( glm::vec3( flX + flHalfWidth, -1.0f, 0.0f ) );
		points.push_back( glm::vec3( flX - flHalfWidth, flY, 0.0f ) );

		points.push_back( glm::vec3( flX + flHalfWidth, flY, 0.0f ) );
		points.push_back( glm::vec3( flX - flHalfWidth, flY, 0.0f ) );
		points.push_back( glm::vec3( flX + flHalfWidth, -1.0f, 0.0f ) );
	}

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, points.size() * sizeof( glm::vec3 ), &points[0], GL_DYNAMIC_DRAW );
}

void VisualizationSpectrum::Draw()
{
	glPointSize( 2.0f );

	glColor4f( 1.0f, 0.0f, 0.0f, 0.55f );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_TRIANGLES, 0, points.size() );
	glDisableVertexAttribArray( 0 );
}