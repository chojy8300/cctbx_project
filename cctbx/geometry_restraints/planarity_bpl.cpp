#include <cctbx/boost_python/flex_fwd.h>
#include <cctbx/geometry_restraints/shared_wrapper_pickle.hpp>

#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/args.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/return_by_value.hpp>
#include <scitbx/array_family/boost_python/shared_wrapper.h>
#include <cctbx/geometry_restraints/planarity.h>
#include <cctbx/geometry_restraints/proxy_select.h>

namespace cctbx { namespace geometry_restraints {
namespace {

  struct planarity_proxy_wrappers : boost::python::pickle_suite
  {
    typedef planarity_proxy w_t;

    static boost::python::tuple
      getinitargs(w_t const& self)
    {
      return boost::python::make_tuple(self.i_seqs,
        self.weights,
        self.origin_id
        );
    }

    static void
    wrap()
    {
      using namespace boost::python;
      typedef return_value_policy<return_by_value> rbv;
      class_<w_t>("planarity_proxy", no_init)
        .def(init<
          af::shared<std::size_t> const&,
          af::shared<double> const&,
          unsigned char>((
            arg("i_seqs"), arg("weights"), arg("origin_id")=0)))
        .def(init<
          af::shared<std::size_t> const&,
          optional_container<af::shared<sgtbx::rt_mx> > const&,
          af::shared<double> const&,
          unsigned char>((
            arg("i_seqs"), arg("sym_ops"), arg("weights"), arg("origin_id")=0)))
        .def(init<
          af::shared<std::size_t> const&,
          w_t const&>((
            arg("i_seqs"), arg("proxy"))))
        .def("scale_weights", &w_t::scale_weights, (arg("factor")))
        .def("sort_i_seqs", &w_t::sort_i_seqs)
        .add_property("i_seqs", make_getter(&w_t::i_seqs, rbv()))
        .add_property("weights", make_getter(&w_t::weights, rbv()))
        .add_property("sym_ops", make_getter(&w_t::sym_ops, rbv()))
        .def_readwrite("origin_id", &w_t::origin_id)
        .def_pickle(planarity_proxy_wrappers())
      ;

      typedef scitbx::af::boost_python::shared_wrapper<w_t> shared_w_t;
      shared_w_t::wrap(
        "shared_planarity_proxy")
        .def("proxy_select",
          (af::shared<w_t>(*)(
            af::const_ref<w_t> const&,
            std::size_t,
            af::const_ref<std::size_t> const&))
              shared_planarity_proxy_select, (
          arg("n_seq"), arg("iselection")))
        .def("proxy_select",
          (af::shared<w_t>(*)(
            af::const_ref<w_t> const&,
            unsigned char))
              shared_proxy_select_origin, (
          arg("origin_id")))
        .def("proxy_remove",
          (af::shared<w_t>(*)(
            af::const_ref<w_t> const&,
            af::const_ref<bool> const&))
              shared_proxy_remove, (
          arg("selection")))
        .def("proxy_remove",
          (af::shared<w_t>(*)(
            af::const_ref<w_t> const&,
            unsigned char))
              shared_proxy_remove, (
          arg("origin_id")))
        .def_pickle(shared_wrapper_pickle_suite< shared_w_t::w_t >())
      ;
    }
  };

  struct planarity_wrappers : boost::python::pickle_suite
  {
    typedef planarity w_t;

    static boost::python::tuple
      getinitargs(w_t const& self)
    {
      return boost::python::make_tuple(self.sites,
        self.weights
        );
    }

    static void
    wrap()
    {
      using namespace boost::python;
      typedef return_value_policy<copy_const_reference> ccr;
      typedef return_internal_reference<> rir;
      typedef return_value_policy<return_by_value> rbv;
      class_<w_t>("planarity", no_init)
        .def(init<
          af::shared<scitbx::vec3<double> > const&,
          af::shared<double> const&>(
            (arg("sites"), arg("weights"))))
        .def(init<af::const_ref<scitbx::vec3<double> > const&,
                  planarity_proxy const&>(
          (arg("sites_cart"), arg("proxy"))))
        .def(init<uctbx::unit_cell const&,
                  af::const_ref<scitbx::vec3<double> > const&,
                  planarity_proxy const&>(
          (arg("unit_cell"), arg("sites_cart"), arg("proxy"))))
        .add_property("sites", make_getter(&w_t::sites, rbv()))
        .add_property("weights", make_getter(&w_t::weights, rbv()))
        .def("deltas", &w_t::deltas, ccr())
        .def("rms_deltas", &w_t::rms_deltas)
        .def("residual", &w_t::residual)
        .def("gradients", &w_t::gradients)
        .def("normal", &w_t::normal)
        .def("lambda_min", &w_t::lambda_min)
        .def("center_of_mass", &w_t::center_of_mass, ccr())
        .def("residual_tensor", &w_t::residual_tensor, ccr())
        .def("eigensystem", &w_t::eigensystem, rir())
        .def_pickle(planarity_wrappers())
      ;
    }
  };

  void
  wrap_all()
  {
    using namespace boost::python;
    planarity_proxy_wrappers::wrap();
    //planarity_sym_proxy_wrappers::wrap();
    planarity_wrappers::wrap();
    def("planarity_deltas_rms",
      (af::shared<double>(*)(
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&))
      planarity_deltas_rms,
      (arg("sites_cart"), arg("proxies")));
    def("planarity_residuals",
      (af::shared<double>(*)(
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&))
      planarity_residuals,
      (arg("sites_cart"), arg("proxies")));
    def("planarity_residual_sum",
      (double(*)(
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&,
        af::ref<scitbx::vec3<double> > const&))
      planarity_residual_sum,
      (arg("sites_cart"), arg("proxies"), arg("gradient_array")));
    def("planarity_deltas_rms",
      (af::shared<double>(*)(
        uctbx::unit_cell const&,
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&))
      planarity_deltas_rms,
      (arg("unit_cell"), arg("sites_cart"), arg("proxies")));
    def("planarity_residuals",
      (af::shared<double>(*)(
        uctbx::unit_cell const&,
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&))
      planarity_residuals,
      (arg("unit_cell"), arg("sites_cart"), arg("proxies")));
    def("planarity_residual_sum",
      (double(*)(
        uctbx::unit_cell const&,
        af::const_ref<scitbx::vec3<double> > const&,
        af::const_ref<planarity_proxy> const&,
        af::ref<scitbx::vec3<double> > const&))
      planarity_residual_sum,
      (arg("unit_cell"), arg("sites_cart"), arg("proxies"), arg("gradient_array")));
  }

} // namespace <anonymous>

namespace boost_python {

  void
  wrap_planarity() { wrap_all(); }

}}} // namespace cctbx::geometry_restraints::boost_python
