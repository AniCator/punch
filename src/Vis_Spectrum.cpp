#include "Vis_Spectrum.h"

CVis_Spectrum::CVis_Spectrum()
{
	glGenBuffers(1, &VBO);
}

CVis_Spectrum::~CVis_Spectrum()
{

}
void CVis_Spectrum::Think(FFT_DataArray fft_data)
{
	points.clear();
	int iSize = FFT_DATAARRAY_SIZE;
	for(int i = 0; i < iSize; i++)
	{
		float flDataNew = fft_data.fft_data[i];

		float flHeight = (float)flDataNew * 3.0f;
		float flModifier = 1.0f;
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
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);
}

void CVis_Spectrum::Draw()
{
	glPointSize(2.0f);

	glColor3f(0.3f,1.0f,0.3f);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, points.size());
	glDisableVertexAttribArray(0);
}