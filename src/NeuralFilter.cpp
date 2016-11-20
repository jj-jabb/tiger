#include "NeuralFilter.h"
#include "AlloyMath.h"
using namespace aly;
namespace tgr {
	ConvolutionFilter::ConvolutionFilter(int width, int height, int kernelX, int kernelY, int features) :NeuralFilter(),kernelX(kernelX),kernelY(kernelY) {
		if (kernelX % 2 == 0 || kernelY % 2 == 0) {
			throw std::runtime_error("Kernel size must be odd.");
		}
		inputLayer=NeuralLayerPtr(new NeuralLayer(width, height));
		outputLayers.resize(features);
	}
	void ConvolutionFilter::attach(NeuralSystem& system) {

		int padX = kernelX / 2;
		int padY = kernelY / 2;
		int width = inputLayer->width;
		int height = inputLayer->height;
		int ow = width - 2 * padX;
		int oh = height - 2 * padY;
		for (NeuralLayerPtr& layer : outputLayers) {
			layer= NeuralLayerPtr(new NeuralLayer(ow, oh));
			layer->setFunction(Tanh());
			system.add(layer);
			std::vector<SignalPtr> signals;
			for (int jj = 0; jj < kernelY; jj++) {
				for (int ii = 0; ii < kernelX; ii++) {
					signals.push_back(SignalPtr(new Signal(RandomUniform(0.0f,1.0f))));
				}
			}
		}
	}
}