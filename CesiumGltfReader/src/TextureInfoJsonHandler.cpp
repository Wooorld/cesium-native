// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "TextureInfoJsonHandler.h"
#include "CesiumGltf/TextureInfo.h"
#include <cassert>
#include <string>

using namespace CesiumGltf;

void TextureInfoJsonHandler::reset(JsonHandler* pParent, TextureInfo* pObject) {
  ExtensibleObjectJsonHandler::reset(pParent);
  this->_pObject = pObject;
}

JsonHandler* TextureInfoJsonHandler::Key(const char* str, size_t /*length*/, bool /*copy*/) {
  using namespace std::string_literals;

  assert(this->_pObject);

  if ("index"s == str) return property(this->_index, this->_pObject->index);
  if ("texCoord"s == str) return property(this->_texCoord, this->_pObject->texCoord);

  return this->ExtensibleObjectKey(str, *this->_pObject);
}
