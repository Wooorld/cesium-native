// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <Cesium3DTiles/MetadataEntity.h>
#include <CesiumJsonReader/DictionaryJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/JsonObjectJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace Cesium3DTilesReader {
class MetadataEntityJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler {
public:
  using ValueType = Cesium3DTiles::MetadataEntity;

  MetadataEntityJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void
  reset(IJsonHandler* pParentHandler, Cesium3DTiles::MetadataEntity* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyMetadataEntity(
      const std::string& objectType,
      const std::string_view& str,
      Cesium3DTiles::MetadataEntity& o);

private:
  Cesium3DTiles::MetadataEntity* _pObject = nullptr;
  CesiumJsonReader::StringJsonHandler _classProperty;
  CesiumJsonReader::DictionaryJsonHandler<
      CesiumUtility::JsonValue,
      CesiumJsonReader::JsonObjectJsonHandler>
      _properties;
};
} // namespace Cesium3DTilesReader
