// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/ExtensibleObject.h"
#include "CesiumGltf/TextureInfo.h"
#include <vector>

namespace CesiumGltf {
    /**
     * @brief A set of parameter values that are used to define the metallic-roughness material model from Physically-Based Rendering (PBR) methodology.
     */
    struct MaterialPBRMetallicRoughness final : public ExtensibleObject {

        /**
         * @brief The material's base color factor.
         *
         * The RGBA components of the base color of the material. The fourth component (A) is the alpha coverage of the material. The `alphaMode` property specifies how alpha is interpreted. These values are linear. If a baseColorTexture is specified, this value is multiplied with the texel values.
         */
        std::vector<double> baseColorFactor;

        /**
         * @brief The base color texture.
         *
         * The base color texture. The first three components (RGB) are encoded with the sRGB transfer function. They specify the base color of the material. If the fourth component (A) is present, it represents the linear alpha coverage of the material. Otherwise, an alpha of 1.0 is assumed. The `alphaMode` property specifies how alpha is interpreted. The stored texels must not be premultiplied.
         */
        TextureInfo baseColorTexture;

        /**
         * @brief The metalness of the material.
         *
         * The metalness of the material. A value of 1.0 means the material is a metal. A value of 0.0 means the material is a dielectric. Values in between are for blending between metals and dielectrics such as dirty metallic surfaces. This value is linear. If a metallicRoughnessTexture is specified, this value is multiplied with the metallic texel values.
         */
        double metallicFactor;

        /**
         * @brief The roughness of the material.
         *
         * The roughness of the material. A value of 1.0 means the material is completely rough. A value of 0.0 means the material is completely smooth. This value is linear. If a metallicRoughnessTexture is specified, this value is multiplied with the roughness texel values.
         */
        double roughnessFactor;

        /**
         * @brief The metallic-roughness texture.
         *
         * The metallic-roughness texture. The metalness values are sampled from the B channel. The roughness values are sampled from the G channel. These values are linear. If other channels are present (R or A), they are ignored for metallic-roughness calculations.
         */
        TextureInfo metallicRoughnessTexture;
    };
}
