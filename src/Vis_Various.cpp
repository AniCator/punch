#include "Vis_Various.h"

CVis_CubeWalking::CVis_CubeWalking(int iFrequency)
{
	iProgressionX = 0;
	iProgressionY = 0;
	flSizeDataOld = 0.0f;
	flProgressionStepsXOld = 0.0f;

	m_iFrequency = iFrequency;
	if(m_iFrequency==0)
	{
		m_iFrequency += 1;
	}

	glGenBuffers(1, &VBO);
}

CVis_CubeWalking::~CVis_CubeWalking()
{

}

void CVis_CubeWalking::Think(FFT_DataArray fft_data)
{
	points.clear();
	float flDataNew = fft_data.fft_data[m_iFrequency];
	float flSizeDataNew = (flDataNew * 0.3f) + (flSizeDataOld * 0.7f);
	flSizeDataOld = flSizeDataNew;
	float flStepSize = ((float)m_iFrequency / (float)FFT_DATAARRAY_SIZE);
	
	float flFaceSizeX = 0.05f + glm::clamp(flSizeDataNew * 5.0f,0.01f,0.1f);
	float flFaceSizeY = 0.05f - glm::clamp(flSizeDataNew * 1.0f,0.01f,0.1f);

	if(flDataNew > (0.2f / (float)m_iFrequency))
		iProgressionX++;

	float flProgressionStepsX = 1.0f + (floor(sin((float)iProgressionX / 30.0f) * 10.0f) / 10.0f);
	flProgressionStepsX = (flProgressionStepsX * 0.6f) + (flProgressionStepsXOld * 0.4f);
	flProgressionStepsXOld = flProgressionStepsX;
	float flProgressionStepsY = 0.025f + (flStepSize * 2.0f);

	//flProgressionStepsX *= 2.0f;
	//flProgressionStepsY *= 2.0f;

	float flFaceOffsetX = -1.0f + flProgressionStepsX - (flFaceSizeX / 2.0f);
	float flFaceOffsetY = 1.0f - (flFaceSizeY / 2.0f) - flProgressionStepsY;

	points.push_back(glm::vec3(flFaceOffsetX + -0.0f, flFaceOffsetY + -flFaceSizeY,0.0f));
	points.push_back(glm::vec3(flFaceOffsetX + flFaceSizeX, flFaceOffsetY + -flFaceSizeY,0.0f));
	points.push_back(glm::vec3(flFaceOffsetX + flFaceSizeX, flFaceOffsetY + flFaceSizeY,0.0f));

	points.push_back(glm::vec3(flFaceOffsetX + -0.0f, flFaceOffsetY + -flFaceSizeY,0.0f));
	points.push_back(glm::vec3(flFaceOffsetX + -0.0f, flFaceOffsetY + flFaceSizeY,0.0f));
	points.push_back(glm::vec3(flFaceOffsetX + flFaceSizeX, flFaceOffsetY + flFaceSizeY,0.0f));

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);
}

void CVis_CubeWalking::Draw()
{
	glPointSize(2.0f);

	float colorR = 1.0f - (float)(iProgressionX % 255) / 255.0f;
	float colorG = (float)(iProgressionX % 255) / 255.0f;
	float colorB = (float)(m_iFrequency / (FFT_DATAARRAY_SIZE / 4));

	glColor3f(colorR,colorG,colorB);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, points.size());
	glDisableVertexAttribArray(0);
}

void CVis_CubeWalking::Reset()
{
	iProgressionX = 0;
	iProgressionY = 0;
}