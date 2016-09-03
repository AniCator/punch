#include "appframework/errordefs.h"
#include "VisLayer.h"

class CLayerManager
{
public:
	CLayerManager();
	~CLayerManager();

	void AddLayer(VisualizationLayer *layer);
	//void RemoveLayer(VisLayer layer);

	void UpdateLayers(FFTDataArray fft_data);
	void DrawLayers();
	void ResetLayers();

protected:
	std::vector<VisualizationLayer*> VisualizationLayers;
};