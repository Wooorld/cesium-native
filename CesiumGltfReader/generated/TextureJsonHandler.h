// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Texture.h"
#include "CesiumJsonReader/IntegerJsonHandler.h"
#include "NamedObjectJsonHandler.h"

namespace CesiumJsonReader {
class ExtensionReaderContext;
}

namespace CesiumGltf {
class TextureJsonHandler : public NamedObjectJsonHandler {
public:
  using ValueType = Texture;

  TextureJsonHandler(
      const CesiumJsonReader::ExtensionReaderContext& context) noexcept;
  void reset(IJsonHandler* pParentHandler, Texture* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyTexture(
      const std::string& objectType,
      const std::string_view& str,
      Texture& o);

private:
  Texture* _pObject = nullptr;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _sampler;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _source;
};
} // namespace CesiumGltf
