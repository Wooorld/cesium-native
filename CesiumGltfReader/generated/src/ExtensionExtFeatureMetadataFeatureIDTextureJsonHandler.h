// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "ExtensionExtFeatureMetadataTextureAccessorJsonHandler.h"

#include <CesiumGltf/ExtensionExtFeatureMetadataFeatureIDTexture.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace CesiumGltfReader {
class ExtensionExtFeatureMetadataFeatureIDTextureJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler {
public:
  using ValueType = CesiumGltf::ExtensionExtFeatureMetadataFeatureIDTexture;

  ExtensionExtFeatureMetadataFeatureIDTextureJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void reset(
      IJsonHandler* pParentHandler,
      CesiumGltf::ExtensionExtFeatureMetadataFeatureIDTexture* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyExtensionExtFeatureMetadataFeatureIDTexture(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::ExtensionExtFeatureMetadataFeatureIDTexture& o);

private:
  CesiumGltf::ExtensionExtFeatureMetadataFeatureIDTexture* _pObject = nullptr;
  CesiumJsonReader::StringJsonHandler _featureTable;
  ExtensionExtFeatureMetadataTextureAccessorJsonHandler _featureIds;
};
} // namespace CesiumGltfReader
