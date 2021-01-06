// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/ExtensibleObject.h"

namespace CesiumGltf {
    /**
     * @brief undefined
     */
    struct MaterialNormalTextureInfo final : public ExtensibleObject {

        /**
         * @brief The scalar multiplier applied to each normal vector of the normal texture.
         *
         * The scalar multiplier applied to each normal vector of the texture. This value scales the normal vector using the formula: `scaledNormal =  normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))`. This value is ignored if normalTexture is not specified. This value is linear.
         */
        double scale;
    };
}
