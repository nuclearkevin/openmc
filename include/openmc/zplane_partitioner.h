#ifndef OPENMC_ZPLANE_PARTITIONER_H
#define OPENMC_ZPLANE_PARTITIONER_H

#include "openmc/bounding_box.h"
#include "openmc/cell.h"
#include "openmc/universe.h"

namespace openmc {

//==============================================================================
//! Speeds up geometry searches by grouping cells in a search tree by z-planes
//! axially.
//==============================================================================

class ZPlanePartitioner : public UniversePartitioner {
public:
  explicit ZPlanePartitioner(const Universe& univ);

  //! Return the list of cells that could contain the given coordinates.
  const vector<int32_t>& get_cells(Position r, Direction u) const override;

private:
  //! A sorted vector of indices to surfaces that partition the universe
  vector<int32_t> surfs_;

  //! Vectors listing the indices of the cells that lie within each partition
  //
  //! There are n+1 partitions with n surfaces.  `partitions_.front()` gives the
  //! cells that lie on the negative side of `surfs_.front()`.
  //! `partitions_.back()` gives the cells that lie on the positive side of
  //! `surfs_.back()`.  Otherwise, `partitions_[i]` gives cells sandwiched
  //! between `surfs_[i-1]` and `surfs_[i]`.
  vector<vector<int32_t>> partitions_;
};

} // namespace openmc

#endif // OPENMC_ZPLANE_PARTITIONER_H
