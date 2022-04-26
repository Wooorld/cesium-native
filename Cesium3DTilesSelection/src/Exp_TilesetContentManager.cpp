#include <Cesium3DTilesSelection/Exp_TileContentLoadInfo.h>
#include <Cesium3DTilesSelection/Exp_TilesetContentLoader.h>
#include <Cesium3DTilesSelection/Exp_TilesetContentManager.h>
#include <Cesium3DTilesSelection/IPrepareRendererResources.h>
#include <CesiumGltfReader/GltfReader.h>

namespace Cesium3DTilesSelection {
namespace {
struct TileLoadResultAndRenderResources {
  TileLoadResult result;
  void* pRenderResources{nullptr};
};

TileLoadResultAndRenderResources postProcessGltf(
    const TileContentLoadInfo& loadInfo,
    const std::shared_ptr<IPrepareRendererResources>& pPrepareRendererResources,
    TileLoadResult&& result) {
  TileRenderContent& renderContent =
      std::get<TileRenderContent>(result.contentKind);
  const TilesetContentOptions& tilesetOptions = loadInfo.contentOptions;
  if (tilesetOptions.generateMissingNormalsSmooth) {
    renderContent.model->generateMissingNormalsSmooth();
  }

  void* pRenderResources = pPrepareRendererResources->prepareInLoadThread(
      *renderContent.model,
      loadInfo.tileTransform);

  return TileLoadResultAndRenderResources{std::move(result), pRenderResources};
}

CesiumAsync::Future<TileLoadResultAndRenderResources> postProcessContent(
    const TileContentLoadInfo& loadInfo,
    TileLoadResult&& result,
    std::shared_ptr<IPrepareRendererResources>&& pPrepareRendererResources) {
  void* pRenderResources = nullptr;
  if (result.state == TileLoadState::ContentLoaded) {
    TileRenderContent* pRenderContent =
        std::get_if<TileRenderContent>(&result.contentKind);
    if (pRenderContent && pRenderContent->model) {
      // Download any external image or buffer urls in the gltf if there are any
      CesiumGltfReader::GltfReaderResult gltfResult{
          std::move(*pRenderContent->model),
          {},
          {}};

      CesiumAsync::HttpHeaders requestHeaders;
      std::string baseUrl;
      if (result.pCompletedRequest) {
        requestHeaders = result.pCompletedRequest->headers();
        baseUrl = result.pCompletedRequest->url();
      }

      CesiumGltfReader::GltfReaderOptions gltfOptions;
      gltfOptions.ktx2TranscodeTargets =
          loadInfo.contentOptions.ktx2TranscodeTargets;

      return CesiumGltfReader::GltfReader::resolveExternalData(
                 loadInfo.asyncSystem,
                 baseUrl,
                 requestHeaders,
                 loadInfo.pAssetAccessor,
                 gltfOptions,
                 std::move(gltfResult))
          .thenInWorkerThread(
              [loadInfo,
               result = std::move(result),
               pPrepareRendererResources =
                   std::move(pPrepareRendererResources)](
                  CesiumGltfReader::GltfReaderResult&& gltfResult) mutable {
                TileRenderContent& renderContent =
                    std::get<TileRenderContent>(result.contentKind);
                renderContent.model = std::move(gltfResult.model);
                return postProcessGltf(
                    loadInfo,
                    pPrepareRendererResources,
                    std::move(result));
              });
    }
  }

  return loadInfo.asyncSystem.createResolvedFuture(
      TileLoadResultAndRenderResources{std::move(result), pRenderResources});
}
} // namespace

TilesetContentManager::TilesetContentManager(
    const TilesetExternals& externals,
    std::vector<CesiumAsync::IAssetAccessor::THeader>&& requestHeaders,
    std::unique_ptr<TilesetContentLoader> pLoader)
    : _externals{externals},
      _requestHeaders{std::move(requestHeaders)},
      _pLoader{std::move(pLoader)} {}

void TilesetContentManager::loadTileContent(
    Tile& tile,
    const TilesetContentOptions& contentOptions) {
  TileContent* pContent = tile.getContent();
  if (!pContent) {
    return;
  }

  if (pContent->getState() != TileLoadState::Unloaded &&
      pContent->getState() != TileLoadState::FailedTemporarily) {
    return;
  }

  TileContentLoadInfo loadInfo{
      _externals.asyncSystem,
      _externals.pAssetAccessor,
      _externals.pLogger,
      contentOptions,
      tile};

  pContent->setState(TileLoadState::ContentLoading);
  _pLoader->loadTileContent(*pContent->getLoader(), loadInfo, _requestHeaders)
      .thenInWorkerThread(
          [pPrepareRendererResources = _externals.pPrepareRendererResources,
           loadInfo](TileLoadResult&& result) mutable {
            return postProcessContent(
                loadInfo,
                std::move(result),
                std::move(pPrepareRendererResources));
          })
      .thenInMainThread([pContent](TileLoadResultAndRenderResources&& pair) {
        TileLoadResult& result = pair.result;
        if (result.state == TileLoadState::ContentLoaded ||
            result.state == TileLoadState::Failed ||
            result.state == TileLoadState::FailedTemporarily) {
          TilesetContentManager::setTileContent(
              *pContent,
              std::move(result.contentKind),
              std::move(result.deferredTileInitializer),
              result.state,
              pair.pRenderResources);
        } else {
          // any states other than the three above are regarded as failed.
          // We will never allow derived class to influence the state of a
          // tile
          TilesetContentManager::setTileContent(
              *pContent,
              std::move(result.contentKind),
              std::move(result.deferredTileInitializer),
              TileLoadState::Failed,
              nullptr);
        }
      });
}

void TilesetContentManager::updateTileContent(Tile& tile) {
  const TileContent* pContent = tile.getContent();
  if (!pContent) {
    return;
  }

  TileLoadState state = pContent->getState();
  switch (state) {
  case TileLoadState::ContentLoaded:
    updateContentLoadedState(tile);
    break;
  default:
    break;
  }
}

bool TilesetContentManager::unloadTileContent(Tile& tile) {
  TileContent* pContent = tile.getContent();
  if (!pContent) {
    return true;
  }

  TileLoadState state = pContent->getState();
  if (state == TileLoadState::Unloaded) {
    return true;
  }

  if (state == TileLoadState::ContentLoading) {
    return false;
  }

  switch (state) {
  case TileLoadState::ContentLoaded:
    unloadContentLoadedState(tile);
    break;
  case TileLoadState::Done:
    unloadDoneState(tile);
    break;
  default:
    break;
  }

  pContent->setContentKind(TileUnknownContent{});
  pContent->setTileInitializerCallback({});
  pContent->setState(TileLoadState::Unloaded);
  return true;
}

void TilesetContentManager::updateRequestHeader(
    const std::string& header,
    const std::string& headerValue) {
  auto authIt = std::find_if(
      _requestHeaders.begin(),
      _requestHeaders.end(),
      [&header](auto& headerPair) { return headerPair.first == header; });
  if (authIt != _requestHeaders.end()) {
    authIt->second = headerValue;
  } else {
    _requestHeaders.emplace_back(header, headerValue);
  }
}

void TilesetContentManager::setTileContent(
    TileContent& content,
    TileContentKind&& contentKind,
    std::function<void(Tile&)>&& tileInitializer,
    TileLoadState state,
    void* pRenderResources) {
  content.setContentKind(std::move(contentKind));
  content.setTileInitializerCallback(std::move(tileInitializer));
  content.setState(state);
  content.setRenderResources(pRenderResources);
}

void TilesetContentManager::updateContentLoadedState(Tile& tile) {
  // initialize this tile content first
  TileContent* pContent = tile.getContent();
  auto& tileInitializer = pContent->getTileInitializerCallback();
  if (tileInitializer) {
    tileInitializer(tile);
  }

  // if tile is external tileset, then it will be refined no matter what
  if (pContent->isExternalContent()) {
    tile.setUnconditionallyRefine();
  }

  // create render resources in the main thread
  const TileRenderContent* pRenderContent = pContent->getRenderContent();
  if (pRenderContent && pRenderContent->model) {
    void* pWorkerRenderResources = pContent->getRenderResources();
    void* pMainThreadRenderResources =
        _externals.pPrepareRendererResources->prepareInMainThread(
            tile,
            pWorkerRenderResources);
    pContent->setRenderResources(pMainThreadRenderResources);
  }

  pContent->setState(TileLoadState::Done);
}

void TilesetContentManager::unloadContentLoadedState(Tile& tile) {
  TileContent* pContent = tile.getContent();
  void* pWorkerRenderResources = pContent->getRenderResources();
  _externals.pPrepareRendererResources->free(
      tile,
      pWorkerRenderResources,
      nullptr);
  pContent->setRenderResources(nullptr);
}

void TilesetContentManager::unloadDoneState(Tile& tile) {
  TileContent* pContent = tile.getContent();
  void* pMainThreadRenderResources = pContent->getRenderResources();
  _externals.pPrepareRendererResources->free(
      tile,
      nullptr,
      pMainThreadRenderResources);
  pContent->setRenderResources(nullptr);
}
} // namespace Cesium3DTilesSelection
