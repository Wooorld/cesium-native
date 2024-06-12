#pragma once

#include "Library.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <optional>

namespace CesiumGeometry {
class Ray;
class Plane;
struct AxisAlignedBox;
class OrientedBoundingBox;
class BoundingSphere;

/**
 * @brief Functions for computing the intersection between geometries such as
 * rays, planes, triangles, and ellipsoids.
 */
class CESIUMGEOMETRY_API IntersectionTests final {
public:
  /**
   * @brief Computes the intersection of a ray and a plane.
   *
   * @param ray The ray.
   * @param plane The plane.
   * @return The point of intersection, or `std::nullopt` if there is no
   * intersection.
   */
  static std::optional<glm::dvec3>
  rayPlane(const Ray& ray, const Plane& plane) noexcept;

  /**
   * @brief Computes the intersection of a ray and a ellipsoid.
   *
   * @param ray The ray.
   * @param radii The radii of ellipsoid.
   * @return The start and stop of intersection, x is start, y is stop, or
   * `std::nullopt` if there is no intersection.
   */
  static std::optional<glm::dvec2>
  rayEllipsoid(const Ray& ray, const glm::dvec3& radii) noexcept;

  /**
   * @brief Determines whether a given point is completely inside a triangle
   * defined by three 2D points.
   *
   * Returns false for degenerate triangles.
   *
   * @param point The point to check.
   * @param triangleVertA The first vertex of the triangle.
   * @param triangleVertB The second vertex of the triangle.
   * @param triangleVertC The third vertex of the triangle.
   * @return Whether the point is within the triangle.
   */
  static bool pointInTriangle(
      const glm::dvec2& point,
      const glm::dvec2& triangleVertA,
      const glm::dvec2& triangleVertB,
      const glm::dvec2& triangleVertC) noexcept;

  /**
   * @brief Determines whether a given point is completely inside a triangle
   * defined by three 3D points.
   *
   * Returns false for degenerate triangles.
   *
   * @param point The point to check.
   * @param triangleVertA The first vertex of the triangle.
   * @param triangleVertB The second vertex of the triangle.
   * @param triangleVertC The third vertex of the triangle.
   * @return Whether the point is within the triangle.
   */
  static bool pointInTriangle(
      const glm::dvec3& point,
      const glm::dvec3& triangleVertA,
      const glm::dvec3& triangleVertB,
      const glm::dvec3& triangleVertC) noexcept;

  /**
   * @brief Determines whether the point is completely inside a triangle defined
   * by three 3D points. If the point is inside, this also outputs the
   * barycentric coordinates for the point.
   *
   * Returns false for degenerate triangles.
   *
   * @param point The point to check.
   * @param triangleVertA The first vertex of the triangle.
   * @param triangleVertB The second vertex of the triangle.
   * @param triangleVertC The third vertex of the triangle.
   * @return Whether the point is within the triangle.
   */
  static bool pointInTriangle(
      const glm::dvec3& point,
      const glm::dvec3& triangleVertA,
      const glm::dvec3& triangleVertB,
      const glm::dvec3& triangleVertC,
      glm::dvec3& barycentricCoordinates) noexcept;

  /**
   * @brief Tests if a ray hits a triangle and returns the hit point.
   *
   * @param ray The ray.
   * @param p0 The first vertex of the triangle.
   * @param p1 The second vertex of the triangle.
   * @param p2 The third vertex of the triangle.
   * @param cullBackFaces Ignore triangles that face away from ray.
   * @return The point of intersection, or `std::nullopt` if there is no
   * intersection.
   */
  static std::optional<glm::dvec3> rayTriangle(
      const Ray& ray,
      const glm::dvec3& p0,
      const glm::dvec3& p1,
      const glm::dvec3& p2,
      bool cullBackFaces = false);

  /**
   * @brief Tests if an infinite ray hits a triangle and returns the parametric
   * hit position.
   *
   * The parameter t is positive if the intersection point is in front of the
   * ray origin, negative if it is behind it, or zero if the two points
   * coincide.
   *
   * @param ray The ray.
   * @param p0 The first vertex of the triangle.
   * @param p1 The second vertex of the triangle.
   * @param p2 The third vertex of the triangle.
   * @param t Returned parametric value, if a hit is detected.
   * @param cullBackFaces Ignore triangles that face away from ray.
   * @return True if a hit occurred, false otherwise.
   */
  static bool rayTriangleParametric(
      const Ray& ray,
      const glm::dvec3& p0,
      const glm::dvec3& p1,
      const glm::dvec3& p2,
      double& t,
      bool cullBackFaces = false);

  /**
   * @brief Computes the intersection of a ray and an axis aligned bounding box.
   *
   * @param ray The ray.
   * @param aabb The axis aligned bounding box.
   * @return The point of intersection, or `std::nullopt` if there is no
   * intersection.
   */
  static std::optional<glm::dvec3>
  rayAABB(const Ray& ray, const AxisAlignedBox& aabb);

  /**
   * @brief Computes the intersection of an infinite ray and an axis aligned
   * bounding box and returns the parametric hit position.
   *
   * The parameter t is positive if the intersection point is in front of the
   * ray origin, negative if it is behind it, or zero if the two points
   * coincide.
   *
   * @param ray The ray.
   * @param aabb The axis aligned bounding box.
   * @param t Returned parametric value, if a hit is detected.
   * @return True if a hit occurred, false otherwise.
   */
  static bool
  rayAABBParametric(const Ray& ray, const AxisAlignedBox& aabb, double& t);

  /**
   * @brief Computes the intersection of a ray and an oriented bounding box.
   *
   * @param ray The ray.
   * @param obb The oriented bounding box.
   * @return The point of intersection, or `std::nullopt` if there is no
   * intersection.
   */
  static std::optional<glm::dvec3>
  rayOBB(const Ray& ray, const OrientedBoundingBox& obb);

  /**
   * @brief Computes the intersection of an infinite ray and an oriented
   * bounding box and returns the parametric hit position.
   *
   * The parameter t is positive if the intersection point is in front of the
   * ray origin, negative if it is behind it, or zero if the two points
   * coincide.
   *
   * @param ray The ray.
   * @param obb The oriented bounding box.
   * @param t Returned parametric value, if a hit is detected.
   * @return True if a hit occurred, false otherwise.
   */
  static bool
  rayOBBParametric(const Ray& ray, const OrientedBoundingBox& obb, double& t);

  /**
   * @brief Computes the intersection of a ray and a bounding sphere.
   *
   * @param ray The ray.
   * @param sphere The bounding sphere.
   * @return The point of intersection, or `std::nullopt` if there is no
   * intersection.
   */
  static std::optional<glm::dvec3>
  raySphere(const Ray& ray, const BoundingSphere& sphere);

  /**
   * @brief Computes the intersection of an infinite ray and a bounding sphere
   * and returns the parametric hit position.
   *
   * The parameter t is positive if the intersection point is in front of the
   * ray origin, negative if it is behind it, or zero if the two points
   * coincide.
   *
   * @param ray The ray.
   * @param sphere The bounding sphere.
   * @param t Returned parametric value, if a hit is detected.
   * @return True if a hit occurred, false otherwise.
   */
  static bool
  raySphereParametric(const Ray& ray, const BoundingSphere& sphere, double& t);
};

} // namespace CesiumGeometry
