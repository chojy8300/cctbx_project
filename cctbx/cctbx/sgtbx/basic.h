// $Id$

#ifndef CCTBX_SGTBX_BASIC_H
#define CCTBX_SGTBX_BASIC_H

#include <exception>
#include <string>
#include <cctbx/error.h>
#include <cctbx/basic/matrixlite.h>

//! Space Group Toolbox namespace.
namespace sgtbx {

  using cctbx::error;

  using MatrixLite::itype::Mx33;
  using MatrixLite::itype::Vec3;

  // Maximum number of representative rotation matrices for
  // 3-dimensional crystallographic space groups.
  static const int nMaxReprRotMx = 24;

  const int STBF = 12; // Seitz           Matrix Translation Base Factor
  const int CRBF = 12; // Change of Basis Matrix Rotation    Base Factor
  const int CTBF = 72; // Change of Basis Matrix Translation Base Factor

  inline void sanity_check() {
    cctbx_assert(STBF % 12 == 0);
    cctbx_assert(CTBF >= 2 * STBF);
    cctbx_assert(CTBF % STBF == 0);
  }

} // namespace sgtbx

#endif // CCTBX_SGTBX_BASIC_H
