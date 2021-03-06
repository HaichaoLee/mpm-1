//! Compute the inverse of a 2d rotation matrix for an orthogonal axis
//! coordinate system
template <>
inline Eigen::Matrix<double, 2, 2> mpm::Geometry<2>::inverse_rotation_matrix(
    const Eigen::Matrix<double, 2, 1>& angles) const {
  // Get cos and sin of angles
  const double cos_alpha_cos_beta = cos(angles(0)) * cos(angles(1));
  const double cos_alpha_sin_beta = cos(angles(0)) * sin(angles(1));
  const double sin_alpha_cos_beta = sin(angles(0)) * cos(angles(1));
  const double sin_alpha_sin_beta = sin(angles(0)) * sin(angles(1));

  // clang-format off
  const Eigen::Matrix<double, 2, 2> rotation_matrix = 
    (Eigen::Matrix<double, 2, 2>() << cos_alpha_cos_beta - sin_alpha_sin_beta,  -cos_alpha_sin_beta - cos_alpha_cos_beta,
                                      sin_alpha_cos_beta + cos_alpha_sin_beta,  -sin_alpha_sin_beta + cos_alpha_cos_beta).finished();                                 
  // clang-format on            

  // inverted rotation matrix
  return rotation_matrix.inverse();
}

//! Compute the inverse of a 3d rotation matrix for an orthogonal axis coordinate system
template <>
inline Eigen::Matrix<double, 3, 3> mpm::Geometry<3>::inverse_rotation_matrix(const 
    Eigen::Matrix<double, 3, 1>& angles) const {

  // Get cos and sin of angles
  const double cos_alpha_cos_beta = cos(angles(0)) * cos(angles(1));
  const double cos_alpha_sin_beta = cos(angles(0)) * sin(angles(1));
  const double sin_alpha_cos_beta = sin(angles(0)) * cos(angles(1));
  const double sin_alpha_sin_beta = sin(angles(0)) * sin(angles(1));
  const double cos_beta_sin_gamma = cos(angles(1)) * sin(angles(2));
  const double sin_beta_sin_gamma = sin(angles(1)) * sin(angles(2));
  const double cos_alpha_sin_gamma = cos(angles(0)) * sin(angles(2));
  const double sin_alpha_sin_gamma = sin(angles(0)) * sin(angles(2));
  const double cos_gamma = cos(angles(2));
  const double sin_gamma = sin(angles(2));

  // clang-format off
  const Eigen::Matrix<double, 3, 3> rotation_matrix = 
    (Eigen::Matrix<double, 3, 3>() << cos_alpha_cos_beta - sin_alpha_sin_beta*cos_gamma,  -cos_alpha_sin_beta - sin_alpha_cos_beta*cos_gamma,   sin_alpha_sin_gamma,
                                      sin_alpha_cos_beta + cos_alpha_sin_beta*cos_gamma,  -sin_alpha_sin_beta + cos_alpha_cos_beta*cos_gamma,  -cos_alpha_sin_gamma,
                                      sin_beta_sin_gamma,                                  cos_beta_sin_gamma,                                  cos_gamma).finished();
  // clang-format on

  // inverted rotation matrix
  return rotation_matrix.inverse();
}

//! Compute the angle between two vectors in radians
template <unsigned Tdim>
inline const double mpm::Geometry<Tdim>::angle_between_vectors(
    const Eigen::Matrix<double, Tdim, 1>& vector_a,
    const Eigen::Matrix<double, Tdim, 1>& vector_b) {
  // angle between two vectors a and b = arccos( a dot b / ||a|| ||b||)
  return acos((vector_a.normalized()).dot((vector_b.normalized())));
}
