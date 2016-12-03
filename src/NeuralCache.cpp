#include "NeuralCache.h"
#include <AlloyFileUtil.h>
using namespace aly;
namespace tgr {
	void CacheElement::load() {
		std::lock_guard<std::mutex> lockMe(accessLock);
		if (!loaded) {
			knowledge.reset(new NeuralKnowledge());
			ReadNeuralKnowledgeFromFile(knowledgeFile, *knowledge);
			//std::cout << "Load: " << knowledge->getFile() << std::endl;
			loaded = true;
		}
	}
	void CacheElement::unload() {
		std::lock_guard<std::mutex> lockMe(accessLock);
		if (loaded) {
			if (writeOnce) {
				WriteNeuralKnowledgeToFile(knowledge->getFile(), *knowledge);
				//std::cout<<"Unload: "<<knowledge->getFile()<<std::endl;
				writeOnce = false;
			}
			knowledge.reset();
			loaded = false;
		}
	}
	void CacheElement::set(const NeuralKnowledge& springl) {
		knowledge.reset(new NeuralKnowledge());
		*knowledge = springl;
		knowledgeFile = springl.getFile();
		loaded = true;
	}
	std::shared_ptr<NeuralKnowledge> CacheElement::getKnowledge() {
		load();
		return knowledge;
	}
	std::shared_ptr<CacheElement> SpringlCache2D::set(int frame, const NeuralKnowledge& springl) {
		std::lock_guard<std::mutex> lockMe(accessLock);
		auto iter = cache.find(frame);
		std::shared_ptr<CacheElement> elem;
		if (iter != cache.end()) {
			elem = iter->second;
		}
		else {
			elem = std::shared_ptr<CacheElement>(new CacheElement());
			cache[frame] = elem;
		}
		elem->set(springl);
		if (elem->isLoaded()) {
			while (loadedList.size() >= maxElements) {
				cache[loadedList.begin()->second]->unload();
				loadedList.erase(loadedList.begin());
			}
			loadedList.insert(std::pair<uint64_t, int>(counter++, frame));
		}
		return elem;
	}
	std::shared_ptr<CacheElement> SpringlCache2D::get(int frame) {
		std::lock_guard<std::mutex> lockMe(accessLock);
		auto iter = cache.find(frame);
		if (iter != cache.end()) {
			std::shared_ptr<CacheElement> elem = iter->second;
			if (!elem->isLoaded()) {
				while (loadedList.size() >= maxElements) {
					cache[loadedList.begin()->second]->unload();
					loadedList.erase(loadedList.begin());
				}
				elem->load();
				loadedList.insert(std::pair<uint64_t, int>(counter++, frame));
			}
			return elem;
		}
		else {
			return std::shared_ptr<CacheElement>();
		}
	}
	CacheElement::~CacheElement() {
		if (FileExists(knowledgeFile)) {
			RemoveFile(knowledgeFile);
			std::string imageFile = GetFileWithoutExtension(knowledgeFile) + ".png";
			if (FileExists(imageFile))RemoveFile(imageFile);
		}
	}
	void SpringlCache2D::clear() {
		std::lock_guard<std::mutex> lockMe(accessLock);
		counter = 0;
		loadedList.clear();
		cache.clear();

	}
}