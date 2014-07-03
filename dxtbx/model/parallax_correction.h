/*
 * parallax_correction.h
 *
 *  Copyright (C) 2013 Diamond Light Source
 *
 *  Author: James Parkhurst
 *
 *  This code is distributed under the BSD license, a copy of which is
 *  included in the root directory of this package.
 */
#ifndef DXTBX_MODEL_PARALLAX_CORRECTION_H
#define DXTBX_MODEL_PARALLAX_CORRECTION_H

#include <scitbx/vec2.h>
#include <scitbx/vec3.h>
#include <cmath>

namespace dxtbx { namespace model {

  using std::sqrt;
  using scitbx::vec2;
  using scitbx::vec3;

  /**
   * Function to perform a parallax correction on a given coordinate. Find the
   * xy coordinate by projecting the vector at the given coordinate la (the
   * attenuation length) into the detector.
   * @param d The distance from the detector to the source along the normal
   * @param la The attenuation length
   * @param xy0 The detector (mm) coordinate of the origin at the normal
   * @param xy The coordinate to correct
   */
  inline
  vec2<double> parallax_correction(double d, double la, vec2<double> xy0,
      vec2<double> xy) {
    vec2<double> xyp = xy - xy0;
    return xy + la * xyp / sqrt(d * d + xyp.length_sq());
  }

  /**
   * Function to perform the inverse parallax correction.
   *
   * N.B This function is an approximation. The real solution is to the
   * equation:
   *
   *    xy' = xy + xy * l / sqrt(h^2 + xy^2)
   *
   * Solving this equation for xy in terms of xy' gives an extremely complicated
   * solution. My maths wan't good enough to get a simpler solution. Might have
   * another look if the approximation becomes an issue but with reasonable
   * geometry, the approximation is correction to more than 3 decimal places.
   *
   * The actual equation used is xy = xy' - xy' * l / sqrt(h^2 + xy'^2)
   *
   * @param d The distance from the detector to the source along the normal
   * @param la The attenuation length
   * @param xy0 The detector (mm) coordinate of the origin at the normal
   * @param xy The coordinate to correct
   */
  inline
  vec2<double> parallax_correction_inv(double d, double la, vec2<double> xy0,
      vec2<double> xy) {
    vec2<double> xyp = xy - xy0;
    return xy - la * xyp / sqrt(d * d + xyp.length_sq());
  }

  /**
   * Function to perform a parallax correction on a given coordinate correctly,
   * given the sensor thickness and so on. X corresponds to the fast direction,
   * Y to the slow direction in input & output. Returns corrected mm position.
   * @param mu Linear attenuation coefficient (mm^-1)
   * @param t0 Sensor thickness (mm)
   * @param fast Detector fast direction
   * @param slow Detector slow direction
   * @param s1 Direction of incoming ray
   */
  inline
  vec2<double> parallax_correction2(double mu, double t0,
                                    vec2<double> xy,
                                    vec3<double> fast,
                                    vec3<double> slow,
                                    vec3<double> s1) {
    vec3<double> normal;
    vec2<double> c_xy;
    double cos_t, o;
    s1 = s1.normalize();
    normal = fast.cross(slow);
    cos_t = s1 * normal;
    o = ((1.0 / mu) - (t0 / cos_t + 1.0 / mu)) * exp(- mu * t0 / cos_t);
    c_xy[0] = xy[0] + (s1 * fast) * o;
    c_xy[1] = xy[1] + (s1 * slow) * o;
    return c_xy;
  }

}} // namespace dxtbx::model

#endif /* DXTBX_MODEL_PARALLAX_CORRECTION_H */
