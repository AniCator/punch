#include "Vis_Spectrum.h"

#include "util/util.h"
#include "ConfigurationManager.h"

VisualizationSpectrum::VisualizationSpectrum()
{
	glGenBuffers(1, &VBO);
	m_flInterpolationSpeed = max( 0.0f, min( 1.0f, ( float ) CConfigurationManager::GetInstance().GetDouble( "spectrum_interpolation_speed" ) ) );

	int iSize = FFT_DATAARRAY_SIZE;
	for( int i = 0; i < iSize; i++ )
	{
		m_DataOld.fft_data[i] = 0.0f;
	}
}

VisualizationSpectrum::~VisualizationSpectrum()
{

}
void VisualizationSpectrum::Think(FFTDataArray fft_data)
{
	points.clear();
	int iSize = FFT_DATAARRAY_SIZE;
	for(int i = 0; i < iSize; i++)
	{
		float flDataNew = fft_data.fft_data[i];
		float flDataOld = m_DataOld.fft_data[i];
		flDataNew = Lerp( flDataOld, flDataNew, m_flInterpolationSpeed );
		m_DataOld.fft_data[i] = flDataNew;

		float flHeight = (float)flDataNew * 5.0f;
		float flModifier = 2.0f;
		float flHalfWidth = (2.0f / (float)iSize) * flModifier * 0.4f;
		float flX = ((2.0f / (float)iSize) * i * flModifier) - 1.0f;
		float flY = flHeight * (2.0f * flModifier) - 1.0f;

		points.push_back(glm::vec3(flX - flHalfWidth,-1.0f,0.0f));
		points.push_back(glm::vec3(flX + flHalfWidth,-1.0f,0.0f));
		points.push_back(glm::vec3(flX - flHalfWidth,flY,0.0f));

		points.push_back(glm::vec3(flX + flHalfWidth,flY,0.0f));
		points.push_back(glm::vec3(flX - flHalfWidth,flY,0.0f));
		points.push_back(glm::vec3(flX + flHalfWidth,-1.0f,0.0f));
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData( GL_ARRAY_BUFFER, points.size() * sizeof( glm::vec3 ), &points[0], GL_DYNAMIC_DRAW );
}

void VisualizationSpectrum::Draw()
{
	glPointSize(2.0f);

	glColor3f(0.3f,1.0f,0.3f);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, points.size());
	glDisableVertexAttribArray(0);
}