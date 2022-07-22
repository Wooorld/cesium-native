#include "CesiumIonTilesetLoader.h"

#include "LayerJsonTerrainLoader.h"
#include "TilesetJsonLoader.h"

#include <CesiumAsync/IAssetAccessor.h>
#include <CesiumAsync/IAssetResponse.h>
#include <CesiumUtility/JsonHelpers.h>
#include <CesiumUtility/Uri.h>

#include <rapidjson/document.h>

#include <unordered_map>

namespace Cesium3DTilesSelection {
namespace {
struct AssetEndpointAttribution {
  std::string html;
  bool collapsible = true;
};

struct AssetEndpoint {
  std::string type;
  std::string url;
  std::string accessToken;
  std::vector<AssetEndpointAttribution> attributions;
};

std::unordered_map<std::string, AssetEndpoint> endpointCache;

std::string createEndpointResource(
    uint32_t ionAssetID,
    const std::string& ionAccessToken,
    const std::string& ionAssetEndpointUrl) {
  std::string ionUrl = fmt::format(
      "{}v1/assets/{}/endpoint?access_token={}",
      ionAssetEndpointUrl,
      ionAssetID,
      ionAccessToken);
  return ionUrl;
}

/**
 * @brief Tries to obtain the `accessToken` from the JSON of the
 * given response.
 *
 * @param pIonResponse The response
 * @return The access token if successful
 */
std::optional<std::string> getNewAccessToken(
    const CesiumAsync::IAssetResponse* pIonResponse,
    const std::shared_ptr<spdlog::logger>& pLogger) {
  const gsl::span<const std::byte> data = pIonResponse->data();
  rapidjson::Document ionResponse;
  ionResponse.Parse(reinterpret_cast<const char*>(data.data()), data.size());
  if (ionResponse.HasParseError()) {
    SPDLOG_LOGGER_ERROR(
        pLogger,
        "Error when parsing Cesium ion response, error code {} at byte offset "
        "{}",
        ionResponse.GetParseError(),
        ionResponse.GetErrorOffset());
    return std::nullopt;
  }
  return CesiumUtility::JsonHelpers::getStringOrDefault(
      ionResponse,
      "accessToken",
      "");
}

CesiumAsync::Future<TilesetContentLoaderResult<CesiumIonTilesetLoader>>
mainThreadLoadTilesetJsonFromAssetEndpoint(
    const TilesetExternals& externals,
    const AssetEndpoint& endpoint,
    uint32_t ionAssetID,
    std::string ionAccessToken,
    std::string ionAssetEndpointUrl,
    CesiumIonTilesetLoader::AuthorizationHeaderChangeListener
        headerChangeListener,
    bool showCreditsOnScreen) {
  std::vector<LoaderCreditResult> credits;
  if (externals.pCreditSystem) {
    credits.reserve(endpoint.attributions.size());
    for (auto& endpointAttribution : endpoint.attributions) {
      bool showOnScreen =
          showCreditsOnScreen || !endpointAttribution.collapsible;
      credits.push_back(
          LoaderCreditResult{endpointAttribution.html, showOnScreen});
    }
  }

  std::vector<CesiumAsync::IAssetAccessor::THeader> requestHeaders;
  requestHeaders.emplace_back(
      "Authorization",
      "Bearer " + endpoint.accessToken);

  return TilesetJsonLoader::createLoader(
             externals,
             endpoint.url,
             requestHeaders)
      .thenImmediately([credits = std::move(credits),
                        requestHeaders,
                        ionAssetID,
                        ionAccessToken = std::move(ionAccessToken),
                        ionAssetEndpointUrl = std::move(ionAssetEndpointUrl),
                        headerChangeListener = std::move(headerChangeListener)](
                           TilesetContentLoaderResult<TilesetJsonLoader>&&
                               tilesetJsonResult) mutable {
        if (tilesetJsonResult.credits.empty()) {
          tilesetJsonResult.credits = std::move(credits);
        } else {
          tilesetJsonResult.credits.insert(
              tilesetJsonResult.credits.end(),
              credits.begin(),
              credits.end());
        }

        TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
        if (!tilesetJsonResult.errors) {
          result.pLoader = std::make_unique<CesiumIonTilesetLoader>(
              ionAssetID,
              std::move(ionAccessToken),
              std::move(ionAssetEndpointUrl),
              std::move(tilesetJsonResult.pLoader),
              std::move(headerChangeListener));
          result.pRootTile = std::move(tilesetJsonResult.pRootTile);
          result.gltfUpAxis = tilesetJsonResult.gltfUpAxis;
          result.credits = std::move(tilesetJsonResult.credits);
          result.requestHeaders = std::move(requestHeaders);
        }
        result.errors = std::move(tilesetJsonResult.errors);
        return result;
      });
}

CesiumAsync::Future<TilesetContentLoaderResult<CesiumIonTilesetLoader>>
mainThreadLoadLayerJsonFromAssetEndpoint(
    const TilesetExternals& externals,
    const TilesetContentOptions& contentOptions,
    const AssetEndpoint& endpoint,
    uint32_t ionAssetID,
    std::string ionAccessToken,
    std::string ionAssetEndpointUrl,
    CesiumIonTilesetLoader::AuthorizationHeaderChangeListener
        headerChangeListener,
    bool showCreditsOnScreen) {
  std::vector<LoaderCreditResult> credits;
  if (externals.pCreditSystem) {
    credits.reserve(endpoint.attributions.size());
    for (auto& endpointAttribution : endpoint.attributions) {
      bool showOnScreen =
          showCreditsOnScreen || !endpointAttribution.collapsible;
      credits.push_back(
          LoaderCreditResult{endpointAttribution.html, showOnScreen});
    }
  }

  std::vector<CesiumAsync::IAssetAccessor::THeader> requestHeaders;
  requestHeaders.emplace_back(
      "Authorization",
      "Bearer " + endpoint.accessToken);

  std::string url =
      CesiumUtility::Uri::resolve(endpoint.url, "layer.json", true);

  return LayerJsonTerrainLoader::createLoader(
             externals,
             contentOptions,
             url,
             requestHeaders,
             showCreditsOnScreen)
      .thenImmediately([credits = std::move(credits),
                        requestHeaders,
                        ionAssetID,
                        ionAccessToken = std::move(ionAccessToken),
                        ionAssetEndpointUrl = std::move(ionAssetEndpointUrl),
                        headerChangeListener = std::move(headerChangeListener)](
                           TilesetContentLoaderResult<LayerJsonTerrainLoader>&&
                               tilesetJsonResult) mutable {
        if (tilesetJsonResult.credits.empty()) {
          tilesetJsonResult.credits = std::move(credits);
        } else {
          tilesetJsonResult.credits.insert(
              tilesetJsonResult.credits.end(),
              credits.begin(),
              credits.end());
        }

        TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
        if (!tilesetJsonResult.errors) {
          result.pLoader = std::make_unique<CesiumIonTilesetLoader>(
              ionAssetID,
              std::move(ionAccessToken),
              std::move(ionAssetEndpointUrl),
              std::move(tilesetJsonResult.pLoader),
              std::move(headerChangeListener));
          result.pRootTile = std::move(tilesetJsonResult.pRootTile);
          result.gltfUpAxis = tilesetJsonResult.gltfUpAxis;
          result.credits = std::move(tilesetJsonResult.credits);
          result.requestHeaders = std::move(requestHeaders);
        }
        result.errors = std::move(tilesetJsonResult.errors);
        return result;
      });
}

CesiumAsync::Future<TilesetContentLoaderResult<CesiumIonTilesetLoader>>
mainThreadHandleEndpointResponse(
    const TilesetExternals& externals,
    std::shared_ptr<CesiumAsync::IAssetRequest>&& pRequest,
    uint32_t ionAssetID,
    std::string&& ionAccessToken,
    std::string&& ionAssetEndpointUrl,
    const TilesetContentOptions& contentOptions,
    CesiumIonTilesetLoader::AuthorizationHeaderChangeListener&&
        headerChangeListener,
    bool showCreditsOnScreen) {
  const CesiumAsync::IAssetResponse* pResponse = pRequest->response();
  const std::string& requestUrl = pRequest->url();
  if (!pResponse) {
    TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
    result.errors.emplace_error(
        fmt::format("No response received for asset request {}", requestUrl));
    return externals.asyncSystem.createResolvedFuture(std::move(result));
  }

  uint16_t statusCode = pResponse->statusCode();
  if (statusCode < 200 || statusCode >= 300) {
    TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
    result.errors.emplace_error(fmt::format(
        "Received status code {} for asset response {}",
        statusCode,
        requestUrl));
    return externals.asyncSystem.createResolvedFuture(std::move(result));
  }

  const gsl::span<const std::byte> data = pResponse->data();

  rapidjson::Document ionResponse;
  ionResponse.Parse(reinterpret_cast<const char*>(data.data()), data.size());

  if (ionResponse.HasParseError()) {
    TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
    result.errors.emplace_error(fmt::format(
        "Error when parsing Cesium ion response JSON, error code {} at byte "
        "offset {}",
        ionResponse.GetParseError(),
        ionResponse.GetErrorOffset()));
    return externals.asyncSystem.createResolvedFuture(std::move(result));
  }

  AssetEndpoint endpoint;
  if (externals.pCreditSystem) {
    const auto attributionsIt = ionResponse.FindMember("attributions");
    if (attributionsIt != ionResponse.MemberEnd() &&
        attributionsIt->value.IsArray()) {

      for (const rapidjson::Value& attribution :
           attributionsIt->value.GetArray()) {
        AssetEndpointAttribution& endpointAttribution =
            endpoint.attributions.emplace_back();
        const auto html = attribution.FindMember("html");
        if (html != attribution.MemberEnd() && html->value.IsString()) {
          endpointAttribution.html = html->value.GetString();
        }
        auto collapsible = attribution.FindMember("collapsible");
        if (collapsible != attribution.MemberEnd() &&
            collapsible->value.IsBool()) {
          endpointAttribution.collapsible = collapsible->value.GetBool();
        }
      }
    }
  }

  std::string url =
      CesiumUtility::JsonHelpers::getStringOrDefault(ionResponse, "url", "");
  std::string accessToken = CesiumUtility::JsonHelpers::getStringOrDefault(
      ionResponse,
      "accessToken",
      "");

  std::string type =
      CesiumUtility::JsonHelpers::getStringOrDefault(ionResponse, "type", "");
  if (type == "TERRAIN") {
    // For terrain resources, we need to append `/layer.json` to the end of the
    // URL.
    url = CesiumUtility::Uri::resolve(url, "layer.json", true);
    endpoint.type = type;
    endpoint.url = url;
    endpoint.accessToken = accessToken;
    endpointCache[requestUrl] = endpoint;
    return mainThreadLoadLayerJsonFromAssetEndpoint(
        externals,
        contentOptions,
        endpoint,
        ionAssetID,
        std::move(ionAccessToken),
        std::move(ionAssetEndpointUrl),
        std::move(headerChangeListener),
        showCreditsOnScreen);
  } else if (type == "3DTILES") {
    endpoint.type = type;
    endpoint.url = url;
    endpoint.accessToken = accessToken;
    endpointCache[requestUrl] = endpoint;
    return mainThreadLoadTilesetJsonFromAssetEndpoint(
        externals,
        endpoint,
        ionAssetID,
        std::move(ionAccessToken),
        std::move(ionAssetEndpointUrl),
        std::move(headerChangeListener),
        showCreditsOnScreen);
  }

  TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
  result.errors.emplace_error(
      fmt::format("Received unsupported asset response type: {}", type));
  return externals.asyncSystem.createResolvedFuture(std::move(result));
}
} // namespace

CesiumIonTilesetLoader::CesiumIonTilesetLoader(
    uint32_t ionAssetID,
    std::string&& ionAccessToken,
    std::string&& ionAssetEndpointUrl,
    std::unique_ptr<TilesetContentLoader>&& pAggregatedLoader,
    std::function<
        void(const std::string& header, const std::string& headerValue)>&&
        headerChangeListener)
    : _refreshTokenState{TokenRefreshState::None},
      _ionAssetID{ionAssetID},
      _ionAccessToken{std::move(ionAccessToken)},
      _ionAssetEndpointUrl{std::move(ionAssetEndpointUrl)},
      _pAggregatedLoader{std::move(pAggregatedLoader)},
      _headerChangeListener{std::move(headerChangeListener)} {}

CesiumAsync::Future<TileLoadResult> CesiumIonTilesetLoader::loadTileContent(
    Tile& tile,
    const TilesetContentOptions& contentOptions,
    const CesiumAsync::AsyncSystem& asyncSystem,
    const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
    const std::shared_ptr<spdlog::logger>& pLogger,
    const std::vector<CesiumAsync::IAssetAccessor::THeader>& requestHeaders) {
  if (_refreshTokenState == TokenRefreshState::Loading) {
    return asyncSystem.createResolvedFuture(TileLoadResult{
        TileUnknownContent{},
        std::nullopt,
        std::nullopt,
        TileLoadResultState::RetryLater,
        nullptr,
        {}});
  } else if (_refreshTokenState == TokenRefreshState::Failed) {
    return asyncSystem.createResolvedFuture(TileLoadResult{
        TileUnknownContent{},
        std::nullopt,
        std::nullopt,
        TileLoadResultState::Failed,
        nullptr,
        {}});
  }

  // TODO: the way this is structured, requests already in progress
  // with the old key might complete after the key has been updated,
  // and there's nothing here clever enough to avoid refreshing the
  // key _again_ in that instance.
  auto refreshTokenInMainThread =
      [this, pLogger, pAssetAccessor, asyncSystem]() {
        this->refreshTokenInMainThread(pLogger, pAssetAccessor, asyncSystem);
      };

  return _pAggregatedLoader
      ->loadTileContent(
          tile,
          contentOptions,
          asyncSystem,
          pAssetAccessor,
          pLogger,
          requestHeaders)
      .thenImmediately([asyncSystem,
                        refreshTokenInMainThread =
                            std::move(refreshTokenInMainThread)](
                           TileLoadResult&& result) mutable {
        // check to see if we need to refresh token
        if (result.pCompletedRequest) {
          auto response = result.pCompletedRequest->response();
          if (response->statusCode() == 401) {
            // retry later
            result.state = TileLoadResultState::RetryLater;
            asyncSystem.runInMainThread(std::move(refreshTokenInMainThread));
          }
        }

        return std::move(result);
      });
}

void CesiumIonTilesetLoader::refreshTokenInMainThread(
    const std::shared_ptr<spdlog::logger>& pLogger,
    const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
    const CesiumAsync::AsyncSystem& asyncSystem) {
  if (_refreshTokenState == TokenRefreshState::Loading) {
    return;
  }

  _refreshTokenState = TokenRefreshState::Loading;

  std::string url = createEndpointResource(
      _ionAssetID,
      _ionAccessToken,
      _ionAssetEndpointUrl);
  pAssetAccessor->get(asyncSystem, url)
      .thenInMainThread(
          [this,
           pLogger](std::shared_ptr<CesiumAsync::IAssetRequest>&& pIonRequest) {
            const CesiumAsync::IAssetResponse* pIonResponse =
                pIonRequest->response();

            if (!pIonResponse) {
              _refreshTokenState = TokenRefreshState::Failed;
              return;
            }

            uint16_t statusCode = pIonResponse->statusCode();
            if (statusCode >= 200 && statusCode < 300) {
              auto accessToken = getNewAccessToken(pIonResponse, pLogger);
              if (accessToken) {
                _headerChangeListener(
                    "Authorization",
                    "Bearer " + *accessToken);

                // update cache with new access token
                auto cacheIt = endpointCache.find(pIonRequest->url());
                if (cacheIt != endpointCache.end()) {
                  cacheIt->second.accessToken = accessToken.value();
                }

                _refreshTokenState = TokenRefreshState::Done;
                return;
              }
            }

            _refreshTokenState = TokenRefreshState::Failed;
          });
}

bool CesiumIonTilesetLoader::updateTileContent(Tile& tile) {
  auto pLoader = tile.getLoader();
  return pLoader->updateTileContent(tile);
}

CesiumAsync::Future<TilesetContentLoaderResult<CesiumIonTilesetLoader>>
CesiumIonTilesetLoader::createLoader(
    const TilesetExternals& externals,
    const TilesetContentOptions& contentOptions,
    uint32_t ionAssetID,
    const std::string& ionAccessToken,
    const std::string& ionAssetEndpointUrl,
    const AuthorizationHeaderChangeListener& headerChangeListener,
    bool showCreditsOnScreen) {
  std::string ionUrl =
      createEndpointResource(ionAssetID, ionAccessToken, ionAssetEndpointUrl);
  auto cacheIt = endpointCache.find(ionUrl);
  if (cacheIt != endpointCache.end()) {
    const auto& endpoint = cacheIt->second;
    if (endpoint.type == "TERRAIN") {
      return mainThreadLoadLayerJsonFromAssetEndpoint(
          externals,
          contentOptions,
          endpoint,
          ionAssetID,
          ionAccessToken,
          ionAssetEndpointUrl,
          headerChangeListener,
          showCreditsOnScreen);
    } else if (endpoint.type == "3DTILES") {
      return mainThreadLoadTilesetJsonFromAssetEndpoint(
          externals,
          endpoint,
          ionAssetID,
          ionAccessToken,
          ionAssetEndpointUrl,
          headerChangeListener,
          showCreditsOnScreen);
    }

    TilesetContentLoaderResult<CesiumIonTilesetLoader> result;
    result.errors.emplace_error(fmt::format(
        "Received unsupported asset response type: {}",
        endpoint.type));
    return externals.asyncSystem.createResolvedFuture(std::move(result));
  } else {
    return externals.pAssetAccessor->get(externals.asyncSystem, ionUrl)
        .thenInMainThread(
            [externals,
             ionAssetID,
             ionAccessToken = ionAccessToken,
             ionAssetEndpointUrl = ionAssetEndpointUrl,
             headerChangeListener = headerChangeListener,
             showCreditsOnScreen,
             contentOptions](std::shared_ptr<CesiumAsync::IAssetRequest>&&
                                 pRequest) mutable {
              return mainThreadHandleEndpointResponse(
                  externals,
                  std::move(pRequest),
                  ionAssetID,
                  std::move(ionAccessToken),
                  std::move(ionAssetEndpointUrl),
                  contentOptions,
                  std::move(headerChangeListener),
                  showCreditsOnScreen);
            });
  }
}
} // namespace Cesium3DTilesSelection
