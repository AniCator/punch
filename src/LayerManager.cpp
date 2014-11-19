#include "LayerManager.h"

CLayerManager::CLayerManager()
{

}

CLayerManager::~CLayerManager()
{

}

void CLayerManager::AddLayer(CVisLayer *layer)
{
	pVisLayers.push_back(layer);
}

void CLayerManager::UpdateLayers(FFT_DataArray fft_data)
{
	for(unsigned int idx = 0; idx < pVisLayers.size(); idx++)
	{
		pVisLayers[idx]->Think(fft_data);
	}
}

void CLayerManager::DrawLayers()
{
	for(unsigned int idx = 0; idx < pVisLayers.size(); idx++)
	{
		pVisLayers[idx]->Draw();
	}
}

void CLayerManager::ResetLayers()
{
	for(unsigned int idx = 0; idx < pVisLayers.size(); idx++)
	{
		pVisLayers[idx]->Reset();
	}
}