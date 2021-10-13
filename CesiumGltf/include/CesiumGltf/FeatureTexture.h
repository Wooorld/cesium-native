// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Library.h"
#include "TextureAccessor.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace CesiumGltf {
/**
 * @brief Features whose property values are stored directly in texture
 * channels. This is not to be confused with feature ID textures which store
 * feature IDs for use with a feature table.
 */
struct CESIUMGLTF_API FeatureTexture final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "FeatureTexture";

  /**
   * @brief The class this feature texture conforms to. The value must be a
   * class ID declared in the `classes` dictionary.
   */
  std::optional<std::string> classProperty;

  /**
   * @brief A dictionary, where each key corresponds to a property ID in the
   * class' `properties` dictionary and each value describes the texture
   * channels containing property values.
   */
  std::unordered_map<std::string, TextureAccessor> properties;
};
} // namespace CesiumGltf
