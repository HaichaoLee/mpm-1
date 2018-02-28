//! Constructor with id and number of nodes
//! \param[in] id Global cell id
//! \param[in] nnodes Number of nodes per cell
//! \tparam Tdim Dimension
template <unsigned Tdim>
mpm::Cell<Tdim>::Cell(Index id, unsigned nnodes) : id_{id}, nnodes_{nnodes} {
  // Check if the dimension is between 1 & 3
  static_assert((Tdim >= 1 && Tdim <= 3), "Invalid global dimension");
  try {
    // Number of nodes should be greater than dimensions
    if (!(nnodes > Tdim)) {
      throw std::runtime_error(
          "Specified number of nodes for a cell is too low");
    }
  } catch (std::exception& exception) {
    std::cerr << exception.what() << '\n';
    std::abort();
  }
}

//! Constructor with id, number of nodes and shapefn
//! \param[in] id Global cell id
//! \param[in] nnodes Number of nodes per cell
//! \param[in] shapefnptr Pointer to a shape function
//! \tparam Tdim Dimension
template <unsigned Tdim>
mpm::Cell<Tdim>::Cell(Index id, unsigned nnodes,
                      const std::shared_ptr<ShapeFn<Tdim>>& shapefnptr)
    : mpm::Cell<Tdim>::Cell(id, nnodes) {

  try {
    if (shapefnptr->nfunctions() >= this->nnodes_) {
      shapefn_ = shapefnptr;
    } else {
      throw std::runtime_error(
          "Specified number of shape functions is not defined");
    }
  } catch (std::exception& exception) {
    std::cerr << exception.what() << '\n';
    std::abort();
  }
}

//! Assign a shape function to cell
//! \param[in] shapefnptr Pointer to a shape function
//! \tparam Tdim Dimension
template <unsigned Tdim>
bool mpm::Cell<Tdim>::shapefn(
    const std::shared_ptr<ShapeFn<Tdim>>& shapefnptr) {
  bool status = false;
  try {
    if (shapefnptr->nfunctions() >= this->nnodes_) {
      shapefn_ = shapefnptr;
      status = true;
    } else {
      throw std::runtime_error(
          "Specified number of shape functions is not defined");
    }
  } catch (std::exception& exception) {
    std::cerr << exception.what() << '\n';
  }
  return status;
}

//! Add a node pointer
//! \param[in] local_id local id of the node
//! \param[in] ptr A shared pointer
//! \retval insertion_status Return the successful addition of a node
//! \tparam Tdim Dimension
template <unsigned Tdim>
bool mpm::Cell<Tdim>::add_node(
    unsigned local_id, const std::shared_ptr<mpm::NodeBase<Tdim>>& node_ptr) {
  bool insertion_status = false;
  try {
    // If number of node ptrs in a cell is less than the maximum number of nodes
    // per cell
    // The local id should be between 0 and maximum number of nodes
    if (nodes_.size() < this->nnodes_ &&
        (local_id >= 0 && local_id < this->nnodes_)) {
      insertion_status = nodes_.insert(local_id, node_ptr);
    } else {
      throw std::runtime_error(
          "Number nodes in a cell exceeds the maximum allowed per cell");
    }
  } catch (std::exception& exception) {
    std::cerr << exception.what() << "\n";
  }
  return insertion_status;
}

//! Add a neighbour cell
//! \param[in] local_id local id of the cell
//! \param[in] ptr A shared pointer to cell
//! \retval insertion_status Return the successful addition of a node
//! \tparam Tdim Dimension
template <unsigned Tdim>
bool mpm::Cell<Tdim>::add_neighbour(
    unsigned local_id, const std::shared_ptr<mpm::Cell<Tdim>>& cell_ptr) {
  bool insertion_status = false;
  try {
    // If number of cell ptrs id is not the current cell id
    if (cell_ptr->id() != this->id()) {
      insertion_status = neighbour_cells_.insert(local_id, cell_ptr);
    } else {
      throw std::runtime_error("Invalid local id of a cell neighbour");
    }
  } catch (std::exception& exception) {
    std::cerr << exception.what() << "\n";
  }
  return insertion_status;
}

//! Add a particle id
//! \param[in] id Global id of a particle
//! \retval status Return the successful addition of a particle id
//! \tparam Tdim Dimension
template <unsigned Tdim>
bool mpm::Cell<Tdim>::add_particle_id(Index id) {
  bool status = false;
  // Check if it is found in the container
  auto itr = std::find(particles_.begin(), particles_.end(), id);

  if (itr == particles_.end()) {
    particles_.emplace_back(id);
    status = true;
  }

  return status;
}

//! Remove a particle id
//! \param[in] id Global id of a particle
//! \retval status Return the successful removal of a particle id
//! \tparam Tdim Dimension
template <unsigned Tdim>
void mpm::Cell<Tdim>::remove_particle_id(Index id) {
  particles_.erase(std::remove(particles_.begin(), particles_.end(), id),
                   particles_.end());
}

//! Compute volume of a 2D cell
//! \retval volume Cell volume / area
//! \tparam Tdim Dimension
template <>
inline void mpm::Cell<2>::compute_volume() {
  try {
    Eigen::VectorXi indices = shapefn_->volume_indices();
    // Quadrilateral
    if (indices.size() == 4) {

      //        b
      // 3 0---------0 2
      //   | \   q / |
      // a |   \  /  | c
      //   |   p \   |
      //   |  /    \ |
      // 0 0---------0 1
      //         d
      const double a = (nodes_[indices[0]]->coordinates() -
                        nodes_[indices[3]]->coordinates())
                           .norm();
      const double b = (nodes_[indices[2]]->coordinates() -
                        nodes_[indices[3]]->coordinates())
                           .norm();
      const double c = (nodes_[indices[1]]->coordinates() -
                        nodes_[indices[2]]->coordinates())
                           .norm();
      const double d = (nodes_[indices[0]]->coordinates() -
                        nodes_[indices[1]]->coordinates())
                           .norm();
      const double p = (nodes_[indices[0]]->coordinates() -
                        nodes_[indices[2]]->coordinates())
                           .norm();
      const double q = (nodes_[indices[1]]->coordinates() -
                        nodes_[indices[3]]->coordinates())
                           .norm();

      // K = 1/4 * sqrt ( 4p^2q^2 - (a^2 + c^2 - b^2 -d^2)^2)
      volume_ =
          0.25 * std::sqrt(4 * p * p * q * q -
                           std::pow((a * a + c * c - b * b - d * d), 2.0));
    } else {
      throw std::runtime_error(
          "Unable to compute volume, number of vertices is incorrect");
    }
  } catch (std::exception& except) {
    std::cout << __FILE__ << __LINE__
              << "Compute volume of a cell: " << except.what() << '\n';
  }
}

//! Compute volume of a 3D cell
//! \retval volume Cell volume / area
//! \tparam Tdim Dimension
template <>
inline void mpm::Cell<3>::compute_volume() {
  try {
    Eigen::VectorXi indices = shapefn_->volume_indices();
    // Hexahedron
    if (indices.size() == 8) {
      // Node numbering as read in by mesh file
      //        d               c
      //          *_ _ _ _ _ _*
      //         /|           /|
      //        / |          / |
      //     a *_ |_ _ _ _ _* b|
      //       |  |         |  |
      //       |  |         |  |
      //       |  *_ _ _ _ _|_ *
      //       | / h        | / g
      //       |/           |/
      //       *_ _ _ _ _ _ *
      //     e               f
      //
      // Calculation of hexahedron volume from
      // https://arc.aiaa.org/doi/pdf/10.2514/3.9013

      const Eigen::Vector3d a = nodes_[indices[7]]->coordinates();
      const Eigen::Vector3d b = nodes_[indices[6]]->coordinates();
      const Eigen::Vector3d c = nodes_[indices[2]]->coordinates();
      const Eigen::Vector3d d = nodes_[indices[3]]->coordinates();
      const Eigen::Vector3d e = nodes_[indices[4]]->coordinates();
      const Eigen::Vector3d f = nodes_[indices[5]]->coordinates();
      const Eigen::Vector3d g = nodes_[indices[1]]->coordinates();
      const Eigen::Vector3d h = nodes_[indices[0]]->coordinates();

      volume_ =
          (1.0 / 12) *
              (a - g).dot(((b - d).cross(c - a)) + ((e - b).cross(f - a)) +
                          ((d - e).cross(h - a))) +
          (1.0 / 12) *
              (b - g).dot(((b - d).cross(c - a)) + ((c - g).cross(c - f))) +
          (1.0 / 12) *
              (e - g).dot(((e - b).cross(f - a)) + ((f - g).cross(h - f))) +
          (1.0 / 12) *
              (d - g).dot(((d - e).cross(h - a)) + ((h - g).cross(h - c)));
    } else {
      throw std::runtime_error(
          "Unable to compute volume, number of vertices is incorrect");
    }
  } catch (std::exception& except) {
    std::cout << __FILE__ << __LINE__
              << "Compute volume of a cell: " << except.what() << '\n';
  }
}

//! Check if a point is in a 2D cell
//! \param[in] point Coordinates of a point
//! \tparam Tdim Dimension
template <>
inline bool mpm::Cell<2>::point_in_cell(
    const Eigen::Matrix<double, 2, 1>& point) {

  // Tolerance for volume / area comparison
  const double tolerance = 1.0E-10;

  // Get the indices of sub-triangles
  Eigen::MatrixXi indices = shapefn_->inhedron_indices();

  // Initialise sub-triangle areas to 0
  double triareas = 0.;

  // Return status
  bool status = false;

  // Iterate over each sub-triangles to calculate the area
  // of each sub-triangle. If the sum of area of all
  // sub-triangle is equal to the area of hexahedron then
  // the point is inside the Hexahedron, if not the point is outside
  for (unsigned i = 0; i < indices.rows(); ++i) {

    // Get the 2 vertices of the sub-triangle
    const Eigen::Matrix<double, 2, 1> a = nodes_[indices(i, 0)]->coordinates();
    const Eigen::Matrix<double, 2, 1> b = nodes_[indices(i, 1)]->coordinates();

    // Compute the area of a triangle
    // Area = |1/2(x1(y2−y3)+x2(y3−y1)+x3(y1−y2))|
    Eigen::Matrix<double, 3, 3> area;
    // clang-format off
    area << 1.  , 1.  , 1.,
            a(0), b(0), point(0),
            a(1), b(1), point(3);
    // clang-format on
    const double triarea = 0.5 * std::fabs(area.determinant());

    triareas += triarea;
    // Optimisation check, if the sub-tetrahedra area exceeds the area of
    // hexahedron, abort and return false (point is outside).
    if ((triareas > volume_) && (std::fabs(triareas - volume_) > tolerance)) {
      return false;
    }
  }
  // Check if the point is inside the hedron
  if (std::fabs(triareas - volume_) < tolerance) {
    status = true;
  }
  return status;
}

//! Check if a point is in a 3D cell
//! \param[in] point Coordinates of a point
//! \tparam Tdim Dimension
template <>
inline bool mpm::Cell<3>::point_in_cell(
    const Eigen::Matrix<double, 3, 1>& point) {

  // Tolerance for volume / area comparison
  const double tolerance = 1.0E-10;

  // Get the indices of sub-tetrahedron
  Eigen::MatrixXi indices = shapefn_->inhedron_indices();

  // Initialise sub-tetrahedra volumes to 0
  double tetvolumes = 0.;

  // Return status
  bool status = false;

  // Iterate over each sub-tetrahedrons to calculate the volume
  // of each sub-tetrahedron. If the sum of volume of all
  // sub-tetrahedron is equal to the volume of hexahedron then
  // the point is inside the Hexahedron, if not the point is outside
  for (unsigned i = 0; i < indices.rows(); ++i) {

    // Get the 3 vertices of the sub-tetrahedron
    const Eigen::Matrix<double, 3, 1> a = nodes_[indices(i, 0)]->coordinates();
    const Eigen::Matrix<double, 3, 1> b = nodes_[indices(i, 1)]->coordinates();
    const Eigen::Matrix<double, 3, 1> c = nodes_[indices(i, 2)]->coordinates();

    // Compute the volume of a tetrahedron
    // Volume = 1/6 | (a - d).((b - d) x (c - d)) |
    const double tetvolume =
        (1. / 6.) * std::fabs((a - point).dot((b - point).cross(c - point)));

    tetvolumes += tetvolume;
    // Optimisation check, if the sub-tetrahedra volume exceeds the volume of
    // hexahedron, abort and return false (point is outside).
    if ((tetvolumes > volume_) &&
        (std::fabs(tetvolumes - volume_) > tolerance)) {
      return false;
    }
  }
  // Check if the point is inside the hedron
  if (std::fabs(tetvolumes - volume_) < tolerance) {
    status = true;
  }
  return status;
}

//! Assign mass to nodes
//! \param[in] xi local coordinates of particle
//! \param[in] pmass mass of particle
//! \tparam Tdim Dimension
template <unsigned Tdim>
void mpm::Cell<Tdim>::assign_mass_to_nodes(const VectorDim& xi,
                                           const Eigen::VectorXd& pmass) {
  Eigen::VectorXd shapefns = shapefn_->shapefn(xi);
  for (unsigned i = 0; i < this->nfunctions(); ++i)
    nodes_[i]->update_mass(shapefns(i) * pmass);
}

//! Assign momentum to nodes
//! \param[in] xi local coordinates of particle
//! \param[in] pmass mass of particle
//! \param[in] pvelocity velocity of particle
//! \tparam Tdim Dimension
template <unsigned Tdim>
void mpm::Cell<Tdim>::assign_momentum_to_nodes(
    const VectorDim& xi, const Eigen::VectorXd& pmass,
    const Eigen::MatrixXd& pvelocity) {
  Eigen::VectorXd shapefns = shapefn_->shapefn(xi);
  Eigen::MatrixXd mass;
  if (pmass.size() == pvelocity.cols()) mass = pmass.asDiagonal();
  for (unsigned i = 0; i < this->nfunctions(); ++i) {
    nodes_[i]->update_momentum(shapefns(i) * mass * pvelocity);
  }
}

//! Assign body force to nodes
//! \param[in] xi Local coordinates of particle
//! \param[in] pmass Mass of particle
//! \param[in] pgravity Gravity of particle
//! \tparam Tdim Dimension
template <unsigned Tdim>
void mpm::Cell<Tdim>::assign_body_force_to_nodes(const VectorDim& xi,
                                                 const Eigen::VectorXd& pmass,
                                                 const VectorDim& pgravity) {
  Eigen::VectorXd shapefns = shapefn_->shapefn(xi);
  for (unsigned i = 0; i < this->nfunctions(); ++i)
    nodes_[i]->update_body_force(shapefns(i) * pgravity * pmass);
}

//! Return velocity at a poing by interpolating from nodes
//! \param[in] xi Local coordinates of point
//! \retval velocity Interpolated velocity
//! \tparam Tdim Dimension
template <unsigned Tdim>
Eigen::VectorXd mpm::Cell<Tdim>::interpolate_velocity(const VectorDim& xi,
                                                      unsigned nphase) {
  Eigen::Matrix<double, Tdim, 1> velocity =
      Eigen::Matrix<double, Tdim, 1>::Zero();
  Eigen::VectorXd shapefns = shapefn_->shapefn(xi);
  for (unsigned i = 0; i < this->nfunctions(); ++i)
    velocity += shapefns(i) * nodes_[i]->velocity(nphase);
  return velocity;
}
