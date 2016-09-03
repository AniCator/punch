#include "LayerManager.h"

CLayerManager::CLayerManager()
{

}

CLayerManager::~CLayerManager()
{

}

void CLayerManager::AddLayer(VisualizationLayer *layer)
{
	VisualizationLayers.push_back(layer);
}

void CLayerManager::UpdateLayers(FFTDataArray fft_data)
{
	for(unsigned int idx = 0; idx < VisualizationLayers.size(); idx++)
	{
		VisualizationLayers[idx]->Think(fft_data);
	}
}

void CLayerManager::DrawLayers()
{
	for(unsigned int idx = 0; idx < VisualizationLayers.size(); idx++)
	{
		VisualizationLayers[idx]->Draw();
	}
}

void CLayerManager::ResetLayers()
{
	for(unsigned int idx = 0; idx < VisualizationLayers.size(); idx++)
	{
		VisualizationLayers[idx]->Reset();
	}
}