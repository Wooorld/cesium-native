// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/MeshPrimitive.h"
#include "CesiumJsonReader/ArrayJsonHandler.h"
#include "CesiumJsonReader/DictionaryJsonHandler.h"
#include "CesiumJsonReader/IntegerJsonHandler.h"

#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

namespace CesiumJsonReader {
class ExtensionReaderContext;
}

namespace CesiumGltf {
class MeshPrimitiveJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler {
public:
  using ValueType = MeshPrimitive;

  MeshPrimitiveJsonHandler(
      const CesiumJsonReader::ExtensionReaderContext& context) noexcept;
  void reset(IJsonHandler* pParentHandler, MeshPrimitive* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyMeshPrimitive(
      const std::string& objectType,
      const std::string_view& str,
      MeshPrimitive& o);

private:
  MeshPrimitive* _pObject = nullptr;
  CesiumJsonReader::DictionaryJsonHandler<
      int32_t,
      CesiumJsonReader::IntegerJsonHandler<int32_t>>
      _attributes;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _indices;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _material;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _mode;
  CesiumJsonReader::ArrayJsonHandler<
      std::unordered_map<std::string, int32_t>,
      CesiumJsonReader::DictionaryJsonHandler<
          int32_t,
          CesiumJsonReader::IntegerJsonHandler<int32_t>>>
      _targets;
};
} // namespace CesiumGltf
