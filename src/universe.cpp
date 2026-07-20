#include "openmc/universe.h"

#include <set>

#include "openmc/hdf5_interface.h"
#include "openmc/particle.h"

namespace openmc {

namespace model {

std::unordered_map<int32_t, int32_t> universe_map;
vector<unique_ptr<Universe>> universes;

} // namespace model

//==============================================================================
// Universe implementation
//==============================================================================

void Universe::to_hdf5(hid_t universes_group) const
{
  // Create a group for this universe.
  auto group = create_group(universes_group, fmt::format("universe {}", id_));

  // Write the geometry representation type.
  write_string(group, "geom_type", "csg", false);

  // Write the contained cells.
  if (cells_.size() > 0) {
    vector<int32_t> cell_ids;
    for (auto i_cell : cells_)
      cell_ids.push_back(model::cells[i_cell]->id_);
    write_dataset(group, "cells", cell_ids);
  }

  close_group(group);
}

bool Universe::find_cell(GeometryState& p) const
{
  const auto& cells {
    !partitioner_ ? cells_ : partitioner_->get_cells(p.r_local(), p.u_local())};

  Position r {p.r_local()};
  Position u {p.u_local()};
  auto surf = p.surface();
  int32_t i_univ = p.lowest_coord().universe();

  for (auto i_cell : cells) {
    if (model::cells[i_cell]->universe_ != i_univ)
      continue;
    // Check if this cell contains the particle
    if (model::cells[i_cell]->contains(r, u, surf)) {
      p.lowest_coord().cell() = i_cell;
      return true;
    }
  }
  return false;
}

BoundingBox Universe::bounding_box() const
{
  BoundingBox bbox = BoundingBox::inverted();
  if (cells_.size() == 0) {
    return {};
  } else {
    for (const auto& cell : cells_) {
      auto& c = model::cells[cell];
      bbox |= c->bounding_box();
    }
  }
  return bbox;
}

} // namespace openmc
