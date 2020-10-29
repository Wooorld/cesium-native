#include "Cesium3DTiles/RasterOverlayCollection.h"

namespace Cesium3DTiles {

    RasterOverlayCollection::RasterOverlayCollection() {

    }

    void RasterOverlayCollection::push_back(std::unique_ptr<RasterOverlay>&& pOverlay) {
        this->_overlays.push_back(std::move(pOverlay));
    }

    void RasterOverlayCollection::createTileProviders(TilesetExternals& tilesetExternals) {
        for (std::unique_ptr<RasterOverlay>& pOverlay : this->_overlays) {
            this->_placeholders.push_back(std::make_unique<RasterOverlayTileProvider>(
                pOverlay.get(),
                tilesetExternals
            ));

            this->_tileProviders.push_back(nullptr);
            this->_quickTileProviders.push_back(this->_placeholders.back().get());

            pOverlay->createTileProvider(tilesetExternals, std::bind(&RasterOverlayCollection::overlayCreated, this, std::placeholders::_1));
        }
    }

    RasterOverlayTileProvider* RasterOverlayCollection::findProviderForPlaceholder(RasterOverlayTileProvider* pPlaceholder) {
        for (size_t i = 0; i < this->_placeholders.size(); ++i) {
            if (this->_placeholders[i].get() == pPlaceholder) {
                if (this->_quickTileProviders[i] != pPlaceholder) {
                    return this->_quickTileProviders[i];
                }
                break;
            }
        }

        return nullptr;
    }

    void RasterOverlayCollection::overlayCreated(std::unique_ptr<RasterOverlayTileProvider>&& pOverlayProvider) {
        if (!pOverlayProvider) {
            return;
        }

        RasterOverlay* pOverlay = pOverlayProvider->getOverlay();

        auto it = std::find_if(this->_placeholders.begin(), this->_placeholders.end(), [pOverlay](const std::unique_ptr<RasterOverlayTileProvider>& pPlaceholder) {
            if (pPlaceholder->getOverlay() == pOverlay) {
                return true;
            }
            return false;
        });

        assert(it != this->_placeholders.end());

        size_t index = it - this->_placeholders.begin();
        this->_quickTileProviders[index] = pOverlayProvider.get();
        this->_tileProviders[index] = std::move(pOverlayProvider);
    }

}