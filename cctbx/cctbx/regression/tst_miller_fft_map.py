from cctbx import crystal
from cctbx import miller
from cctbx import maptbx
from cctbx import sgtbx
from cctbx.development import random_structure
from cctbx.development import debug_utils
from cctbx.development import structure_factor_utils
from cctbx.array_family import flex
from scitbx import fftpack
from scitbx.test_utils import approx_equal
import sys

def check_peaks(structure, peak_sites, max_min_dist):
  for scatterer in structure.scatterers():
    site_symmetry = structure.site_symmetry(scatterer.site)
    equiv_sites = sgtbx.sym_equiv_sites(site_symmetry)
    min_dist = None
    for peak_site in peak_sites:
      dist_info = sgtbx.min_sym_equiv_distance_info(equiv_sites, peak_site)
      if (min_dist is None):
        min_dist = dist_info.dist()
      else:
        min_dist = min(min_dist, dist_info.dist())
    assert min_dist <= max_min_dist, (min_dist, max_min_dist)

def run_test(space_group_info, n_elements=5, d_min=1.5,
             grid_resolution_factor=1./3, max_prime=5, verbose=0):
  structure = random_structure.xray_structure(
    space_group_info,
    elements=["Si"]*n_elements,
    volume_per_atom=200,
    min_distance=3.,
    general_positions_only=00000)
  miller_set_f_obs = miller.build_set(
    crystal_symmetry=structure,
    anomalous_flag=00000,
    d_min=d_min)
  f_obs = miller_set_f_obs.structure_factors_from_scatterers(
    xray_structure=structure,
    direct=0001).f_calc()
  structure_factor_utils.check_phase_restrictions(f_obs, verbose=verbose)
  if (0 or verbose):
    f_obs.show_summary()
  if (0 or verbose):
    f_obs.show_array()
  fft_map = f_obs.fft_map(
    resolution_factor=grid_resolution_factor,
    symmetry_flags=maptbx.use_space_group_symmetry)
  real_map = maptbx.copy(
    fft_map.real_map(),
    flex.grid(fft_map.real_map().focus()))
  grid_tags = maptbx.grid_tags(real_map.focus())
  grid_tags.build(
    fft_map.space_group_info().type(),
    fft_map.symmetry_flags())
  assert grid_tags.n_grid_misses() == 0
  assert grid_tags.verify(real_map)
  peak_list = maptbx.peak_list(
    data=real_map,
    tags=grid_tags.tag_array(),
    peak_search_level=1,
    max_peaks=2*n_elements)
  assert peak_list.gridding() == real_map.focus()
  peak_sites = flex.vec3_double()
  peak_heights = flex.double()
  for entry in peak_list.entries():
    peak_sites.append(
      [float(entry.index[i]) / peak_list.gridding()[i] for i in xrange(3)])
    peak_heights.append(entry.value)
  check_peaks(structure, peak_sites, d_min * grid_resolution_factor)
  crystal_gridding_tags = fft_map.tags()
  cluster_analysis = maptbx.peak_cluster_analysis(
    peak_list=peak_list,
    special_position_settings=structure,
    general_positions_only=00000,
    min_cross_distance=2,
    max_clusters=n_elements).all()
  check_peaks(
    structure,
    cluster_analysis.sites(),
    cluster_analysis.min_cross_distance() + d_min * grid_resolution_factor)

def run_call_back(flags, space_group_info):
  run_test(space_group_info, verbose=flags.Verbose)

def run():
  debug_utils.parse_options_loop_space_groups(sys.argv[1:], run_call_back)

if (__name__ == "__main__"):
  run()
