// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "AccessorSparseJsonHandler.h"
#include "CesiumGltf/AccessorSparse.h"
#include <cassert>
#include <string>

using namespace CesiumGltf;

void AccessorSparseJsonHandler::reset(JsonHandler* pParent, AccessorSparse* pObject) {
  ExtensibleObjectJsonHandler::reset(pParent);
  this->_pObject = pObject;
}

JsonHandler* AccessorSparseJsonHandler::Key(const char* str, size_t /*length*/, bool /*copy*/) {
  using namespace std::string_literals;

  assert(this->_pObject);

  if ("count"s == str) return property(this->_count, this->_pObject->count);
  if ("indices"s == str) return property(this->_indices, this->_pObject->indices);
  if ("values"s == str) return property(this->_values, this->_pObject->values);

  return this->ExtensibleObjectKey(str, *this->_pObject);
}
