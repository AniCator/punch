#include "appframework/errordefs.h"
#include "VisLayer.h"

class CLayerManager
{
public:
	CLayerManager();
	~CLayerManager();

	void AddLayer(VisualizationLayer *layer);
	//void RemoveLayer(VisLayer layer);

	void Clear();

	void UpdateLayers(DataArrayFFT fft_data);
	void DrawLayers();
	void ResetLayers();

protected:
	std::vector<VisualizationLayer*> VisualizationLayers;
};