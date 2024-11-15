// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <unordered_map>

namespace CesiumGltf {
/**
 * @brief glTF extension defines instance attributes for a node with a mesh.
 */
struct CESIUMGLTF_API ExtensionExtMeshGpuInstancing final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "ExtensionExtMeshGpuInstancing";
  static inline constexpr const char* ExtensionName = "EXT_mesh_gpu_instancing";

  /**
   * @brief A dictionary object, where each key corresponds to instance
   * attribute and each value is the index of the accessor containing
   * attribute's data. Attributes TRANSLATION, ROTATION, SCALE define instance
   * transformation. For "TRANSLATION" the values are FLOAT_VEC3's specifying
   * translation along the x, y, and z axes. For "ROTATION" the values are
   * VEC4's specifying rotation as a quaternion in the order (x, y, z, w), where
   * w is the scalar, with component type `FLOAT` or normalized integer. For
   * "SCALE" the values are FLOAT_VEC3's specifying scaling factors along the x,
   * y, and z axes.
   */
  std::unordered_map<std::string, int32_t> attributes;

  /**
   * @brief Calculates the size in bytes of this object, including the contents
   * of all collections, pointers, and strings. This will NOT include the size
   * of any extensions attached to the object. Calling this method may be slow
   * as it requires traversing the object's entire structure.
   */
  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += sizeof(ExtensionExtMeshGpuInstancing);
    accum += CesiumUtility::ExtensibleObject::getSizeBytes() -
             sizeof(CesiumUtility::ExtensibleObject);
    accum += this->attributes.bucket_count() *
             (sizeof(std::string) + sizeof(int32_t));
    for (const auto& [k, v] : this->attributes) {
      accum += k.capacity() * sizeof(char) - sizeof(std::string);
      accum += sizeof(int32_t);
    }
    return accum;
  }
};
} // namespace CesiumGltf
