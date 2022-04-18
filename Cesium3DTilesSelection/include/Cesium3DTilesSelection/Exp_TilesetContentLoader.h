#pragma once

#include <Cesium3DTilesSelection/Exp_TileContent.h>
#include <Cesium3DTilesSelection/Exp_TileUserDataStorage.h>
#include <Cesium3DTilesSelection/TilesetExternals.h>
#include <Cesium3DTilesSelection/TilesetOptions.h>
#include <CesiumAsync/Future.h>

#include <memory>

namespace Cesium3DTilesSelection {
class Tile;

struct TileLoadResult {
  TileContentKind contentKind;
  TileLoadState state;
  uint16_t httpStatusCode;
};

enum class FailedTemporarilyTileAction {
  GiveUp,
  Retry,
  Wait
};

using FailedTemporarilyTileCallback = std::function<FailedTemporarilyTileAction(Tile &tile)>;

class TilesetContentLoader {
public:
  TilesetContentLoader(const TilesetExternals& externals);

  virtual ~TilesetContentLoader() = default;

  void loadTileContent(Tile& tile, const TilesetContentOptions& contentOptions);

  void updateTileContent(Tile& tile);

  bool unloadTileContent(Tile& tile);

  void setTileFailedTemporarilyCallback(FailedTemporarilyTileCallback callback);

private:
  static void setTileContentState(
      TileContent& content,
      TileContentKind&& contentKind,
      TileLoadState state,
      uint16_t httpStatusCode,
      void* pRenderResources);

  void resetTileContent(TileContent& content);

  void deleteAllTileUserData(TileContent& content);

  void updateFailedTemporarilyState(Tile& tile);

  void updateContentLoadedState(Tile& tile);

  void updateDoneState(Tile& tile);

  void unloadContentLoadedState(Tile& tile);

  void unloadDoneState(Tile& tile);

  virtual CesiumAsync::Future<TileLoadResult> doLoadTileContent(
      Tile& tile,
      const TilesetContentOptions& contentOptions) = 0;

  virtual void doProcessLoadedContent(Tile& tile) = 0;

  virtual void doUpdateTileContent(Tile& tile) = 0;

  virtual bool doUnloadTileContent(Tile& tile) = 0;

  FailedTemporarilyTileCallback _failedTemporarilyCallback;
  TileUserDataStorage _customDataStorage;

protected:
  template <class UserData, class... Args>
  UserData& createUserData(Tile& tile, Args&&... args) {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    return _customDataStorage.createUserData<UserData>(
        handle,
        std::forward<Args>(args)...);
  }

  template <class UserData> UserData& getUserData(Tile& tile) {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    return _customDataStorage.getUserData<UserData>(handle);
  }

  template <class UserData> const UserData& getUserData(Tile& tile) const {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    return _customDataStorage.getUserData<UserData>(handle);
  }

  template <class UserData> UserData* tryGetUserData(Tile& tile) {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    return _customDataStorage.tryGetUserData<UserData>(handle);
  }

  template <class UserData> const UserData* tryGetUserData(Tile& tile) const {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    return _customDataStorage.tryGetUserData<UserData>(handle);
  }

  template <class UserData> void deleteUserData(Tile& tile) {
    auto handle = tile.exp_GetContent()->getLoaderCustomDataHandle();
    _customDataStorage.deleteUserData<UserData>(handle);
  }
  TilesetExternals _externals;
};
} // namespace Cesium3DTilesSelection
