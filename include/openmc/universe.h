#ifndef OPENMC_UNIVERSE_H
#define OPENMC_UNIVERSE_H

#include "openmc/bounding_box.h"
#include "openmc/cell.h"

namespace openmc {

#ifdef OPENMC_DAGMC_ENABLED
class DAGUniverse;
#endif

class GeometryState;
class Universe;

namespace model {

extern std::unordered_map<int32_t, int32_t> universe_map;
extern vector<unique_ptr<Universe>> universes;

} // namespace model

//==============================================================================
//! A pure-virtual class for a universe partitioner. The derived classes are
//! expected to partition universes into lists of cells that may contain a given
//! spatial coordinate.
//==============================================================================

class UniversePartitioner {
public:
  UniversePartitioner() = default;

  //! Return the list of cells that could contain the given coordinates.
  virtual const vector<int32_t>& get_cells(Position r, Direction u) const = 0;
};

//==============================================================================
//! A geometry primitive that fills all space and contains cells.
//==============================================================================

class Universe {
public:
  int32_t id_;            //!< Unique ID
  vector<int32_t> cells_; //!< Cells within this universe
  int32_t n_instances_;   //!< Number of instances of this universe

  //! \brief Write universe information to an HDF5 group.
  //! \param group_id An HDF5 group id.
  virtual void to_hdf5(hid_t group_id) const;

  virtual bool find_cell(GeometryState& p) const;

  BoundingBox bounding_box() const;

  /* By default, universes are CSG universes. The DAGMC
   * universe overrides standard behaviors, and in the future,
   * other things might too.
   */
  virtual GeometryType geom_type() const { return GeometryType::CSG; }

  unique_ptr<UniversePartitioner> partitioner_;
};

} // namespace openmc

#endif // OPENMC_UNIVERSE_H
