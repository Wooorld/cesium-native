// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Class.h"
#include "CesiumGltf/Enum.h"
#include "CesiumGltf/Library.h"

#include <CesiumUtility/SharedAsset.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace CesiumGltf {
/**
 * @brief An object defining classes and enums.
 */
struct CESIUMGLTF_API Schema final : public CesiumUtility::SharedAsset<Schema> {
  static inline constexpr const char* TypeName = "Schema";

  /**
   * @brief Unique identifier for the schema. Schema IDs must be alphanumeric
   * identifiers matching the regular expression `^[a-zA-Z_][a-zA-Z0-9_]*$`.
   */
  std::string id;

  /**
   * @brief The name of the schema, e.g. for display purposes.
   */
  std::optional<std::string> name;

  /**
   * @brief The description of the schema.
   */
  std::optional<std::string> description;

  /**
   * @brief Application-specific version of the schema.
   */
  std::optional<std::string> version;

  /**
   * @brief A dictionary, where each key is a class ID and each value is an
   * object defining the class. Class IDs must be alphanumeric identifiers
   * matching the regular expression `^[a-zA-Z_][a-zA-Z0-9_]*$`.
   */
  std::unordered_map<std::string, CesiumGltf::Class> classes;

  /**
   * @brief A dictionary, where each key is an enum ID and each value is an
   * object defining the values for the enum. Enum IDs must be alphanumeric
   * identifiers matching the regular expression `^[a-zA-Z_][a-zA-Z0-9_]*$`.
   */
  std::unordered_map<std::string, CesiumGltf::Enum> enums;

  /**
   * @brief Calculates the size in bytes of this object, including the contents
   * of all collections, pointers, and strings. This will NOT include the size
   * of any extensions attached to the object. Calling this method may be slow
   * as it requires traversing the object's entire structure.
   */
  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += sizeof(Schema);
    accum += CesiumUtility::SharedAsset<Schema>::getSizeBytes() -
             sizeof(CesiumUtility::SharedAsset<Schema>);
    accum += this->id.capacity() * sizeof(char);
    if (this->name) {
      accum += this->name->capacity() * sizeof(char);
    }
    if (this->description) {
      accum += this->description->capacity() * sizeof(char);
    }
    if (this->version) {
      accum += this->version->capacity() * sizeof(char);
    }
    accum += this->classes.bucket_count() *
             (sizeof(std::string) + sizeof(CesiumGltf::Class));
    for (const auto& [k, v] : this->classes) {
      accum += k.capacity() * sizeof(char) - sizeof(std::string);
      accum += v.getSizeBytes() - sizeof(CesiumGltf::Class);
    }
    accum += this->enums.bucket_count() *
             (sizeof(std::string) + sizeof(CesiumGltf::Enum));
    for (const auto& [k, v] : this->enums) {
      accum += k.capacity() * sizeof(char) - sizeof(std::string);
      accum += v.getSizeBytes() - sizeof(CesiumGltf::Enum);
    }
    return accum;
  }
};
} // namespace CesiumGltf
