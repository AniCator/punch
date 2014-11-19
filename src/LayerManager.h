#include "appframework/errordefs.h"
#include "VisLayer.h"

class CLayerManager
{
public:
	CLayerManager();
	~CLayerManager();

	void AddLayer(CVisLayer *layer);
	//void RemoveLayer(VisLayer layer);

	void UpdateLayers(FFT_DataArray fft_data);
	void DrawLayers();
	void ResetLayers();

protected:
	std::vector<CVisLayer*> pVisLayers;
};