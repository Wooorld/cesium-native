// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "ExtensibleObjectJsonHandler.h"
#include "IntegerJsonHandler.h"

namespace CesiumGltf {
  struct TextureInfo;

  class TextureInfoJsonHandler final : public ExtensibleObjectJsonHandler {
  public:
    void reset(JsonHandler* pHandler, TextureInfo* pObject);
    virtual JsonHandler* Key(const char* str, size_t length, bool copy) override;

  private:

    TextureInfo* _pObject;
    IntegerJsonHandler<int32_t> _index;
    IntegerJsonHandler<int64_t> _texCoord;
  };
}
