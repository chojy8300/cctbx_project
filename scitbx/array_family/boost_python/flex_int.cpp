/* Copyright (c) 2001-2002 The Regents of the University of California
   through E.O. Lawrence Berkeley National Laboratory, subject to
   approval by the U.S. Department of Energy.
   See files COPYRIGHT.txt and LICENSE.txt for further details.

   Revision history:
     2002 Aug: Created (R.W. Grosse-Kunstleve)
 */

#include <scitbx/array_family/boost_python/flex_wrapper.h>
#include <scitbx/array_family/boost_python/flex_pickle_single_buffered.h>
#include <scitbx/array_family/boost_python/ref_c_grid_flex_conversions.h>
#include <scitbx/array_family/accessors/c_grid.h>

namespace scitbx { namespace af { namespace boost_python {

  void wrap_flex_int()
  {
    flex_wrapper<int>::integer("int", boost::python::scope())
      .def_pickle(flex_pickle_single_buffered<int, 12>());

    ref_c_grid_flex_conversions<int, c_grid<2> >();
    ref_c_grid_flex_conversions<int, c_grid<3> >();
  }

}}} // namespace scitbx::af::boost_python
