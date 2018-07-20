#include <iostream>
#include <limits>

#include "catch.hpp"

#include "cell.h"
#include "hex_shapefn.h"
#include "node.h"
#include "particle.h"
#include "quad_shapefn.h"
#include "shapefn.h"

//! \brief Check particle class for 1D case
TEST_CASE("Particle is checked for 1D case", "[particle][1D]") {
  // Dimension
  const unsigned Dim = 1;
  // Dimension
  const unsigned Dof = 1;
  // Phases
  const unsigned Nphases = 1;
  // Phase
  const unsigned Phase = 0;

  // Coordinates
  Eigen::Matrix<double, 1, 1> coords;
  coords.setZero();

  //! Check for id = 0
  SECTION("Particle id is zero") {
    mpm::Index id = 0;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == 0);
    REQUIRE(particle->status() == true);
  }

  SECTION("Particle id is positive") {
    //! Check for id is a positive value
    mpm::Index id = std::numeric_limits<mpm::Index>::max();
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == std::numeric_limits<mpm::Index>::max());
    REQUIRE(particle->status() == true);
  }

  //! Construct with id, coordinates and status
  SECTION("Particle with id, coordinates, and status") {
    mpm::Index id = 0;
    bool status = true;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords, status);
    REQUIRE(particle->id() == 0);
    REQUIRE(particle->status() == true);
    particle->assign_status(false);
    REQUIRE(particle->status() == false);
  }

  //! Test coordinates function
  SECTION("coordinates function is checked") {
    mpm::Index id = 0;
    const double Tolerance = 1.E-7;

    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check for coordinates being zero
    auto coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
    REQUIRE(coordinates.size() == Dim);

    // Check for negative value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = -1. * std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);

    // Check for positive value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);
  }

  SECTION("Check particle properties") {
    mpm::Index id = 0;
    const double Tolerance = 1.E-7;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check mass
    REQUIRE(particle->mass(Phase) == Approx(0.0).epsilon(Tolerance));
    double mass = 100.5;
    particle->assign_mass(Phase, mass);
    REQUIRE(particle->mass(Phase) == Approx(100.5).epsilon(Tolerance));

    // Check stress
    Eigen::Matrix<double, 6, 1> stress;
    for (unsigned i = 0; i < stress.size(); ++i) stress(i) = 17.51;

    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(0.).epsilon(Tolerance));

    particle->assign_stress(Phase, stress);
    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(17.51).epsilon(Tolerance));

    // Check velocity
    Eigen::VectorXd velocity;
    velocity.resize(Dim);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 17.51;

    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) == Approx(0.).epsilon(Tolerance));

    bool status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == true);
    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) == Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of velocity
    velocity.resize(Dim * 2);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 17.51;
    status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == false);

    // Check momentum
    Eigen::VectorXd momentum;
    momentum.resize(Dim);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 17.51;

    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) == Approx(0.).epsilon(Tolerance));

    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == true);
    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) == Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of momentum
    momentum.resize(Dim * 2);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 17.51;
    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == false);

    // Check acceleration
    Eigen::VectorXd acceleration;
    acceleration.resize(Dim);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 17.51;

    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(0.).epsilon(Tolerance));

    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == true);
    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of acceleration
    acceleration.resize(Dim * 2);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 17.51;
    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == false);
  }
}

//! \brief Check particle class for 2D case
TEST_CASE("Particle is checked for 2D case", "[particle][2D]") {
  // Dimension
  const unsigned Dim = 2;
  // Degree of freedom
  const unsigned Dof = 2;
  // Number of phases
  const unsigned Nphases = 1;
  // Phase
  const unsigned Phase = 0;
  // Number of nodes per cell
  const unsigned Nnodes = 4;
  // Tolerance
  const double Tolerance = 1.E-7;
  // Coordinates
  Eigen::Vector2d coords;
  coords.setZero();

  //! Check for id = 0
  SECTION("Particle id is zero") {
    mpm::Index id = 0;
    auto particle = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == 0);
  }

  SECTION("Particle id is positive") {
    //! Check for id is a positive value
    mpm::Index id = std::numeric_limits<mpm::Index>::max();
    auto particle = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == std::numeric_limits<mpm::Index>::max());
  }

  //! Test coordinates function
  SECTION("coordinates function is checked") {
    mpm::Index id = 0;
    auto particle = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    //! Check for coordinates being zero
    auto coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
    REQUIRE(coordinates.size() == Dim);

    //! Check for negative value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = -1. * std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);

    //! Check for positive value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);
  }

  SECTION("Add a pointer to a cell to particle") {
    // Add particle
    mpm::Index id = 0;
    coords << 0.75, 0.75;
    auto particle = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check particle coordinates
    auto coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    // Shape function
    std::shared_ptr<mpm::ShapeFn<Dim>> shapefn =
        std::make_shared<mpm::QuadrilateralShapeFn<Dim, 4>>();

    // Create cell
    auto cell = std::make_shared<mpm::Cell<Dim>>(10, Nnodes, shapefn);
    // Add nodes to cell
    coords << 0.5, 0.5;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);

    coords << 1.5, 0.5;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);

    coords << 1.5, 1.5;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);

    coords << 0.5, 1.5;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);
    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);
    REQUIRE(cell->nnodes() == 4);

    // Initialise cell properties
    cell->initialise();

    // Add cell to particle
    REQUIRE(cell->status() == false);
    particle->assign_cell(cell);
    REQUIRE(cell->status() == true);
    REQUIRE(particle->cell_id() == 10);

    // Check if cell is initialised
    REQUIRE(cell->is_initialised() == true);

    // Check reference location
    coords << -0.5, -0.5;
    particle->compute_reference_location();
    auto ref_coordinates = particle->reference_location();
    for (unsigned i = 0; i < ref_coordinates.size(); ++i)
      REQUIRE(ref_coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
  }

  SECTION("Check particle properties") {
    mpm::Index id = 0;
    const double Tolerance = 1.E-7;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check mass
    REQUIRE(particle->mass(Phase) == Approx(0.0).epsilon(Tolerance));
    double mass = 100.5;
    particle->assign_mass(Phase, mass);
    REQUIRE(particle->mass(Phase) == Approx(100.5).epsilon(Tolerance));

    // Check stress
    Eigen::Matrix<double, 6, 1> stress;
    for (unsigned i = 0; i < stress.size(); ++i) stress(i) = 17.52;

    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(0.).epsilon(Tolerance));

    particle->assign_stress(Phase, stress);
    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(17.52).epsilon(Tolerance));

    // Check velocity
    Eigen::VectorXd velocity;
    velocity.resize(Dim);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 19.745;

    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) == Approx(0.).epsilon(Tolerance));

    bool status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == true);
    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) ==
              Approx(19.745).epsilon(Tolerance));
    // Check for incorrect dimension of velocity
    velocity.resize(Dim * 2);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 19.745;
    status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == false);

    // Check momentum
    Eigen::VectorXd momentum;
    momentum.resize(Dim);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 19.745;

    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) == Approx(0.).epsilon(Tolerance));

    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == true);
    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) ==
              Approx(19.745).epsilon(Tolerance));
    // Check for incorrect dimension of momentum
    momentum.resize(Dim * 2);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 19.745;
    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == false);

    // Check acceleration
    Eigen::VectorXd acceleration;
    acceleration.resize(Dim);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 19.745;

    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(0.).epsilon(Tolerance));

    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == true);
    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(19.745).epsilon(Tolerance));
    // Check for incorrect dimension of acceleration
    acceleration.resize(Dim * 2);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 19.745;
    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == false);
  }
}

//! \brief Check particle class for 3D case
TEST_CASE("Particle is checked for 3D case", "[particle][3D]") {
  // Dimension
  const unsigned Dim = 3;
  // Dimension
  const unsigned Dof = 6;
  // Nnumber of phases
  const unsigned Nphases = 1;
  // Phase
  const unsigned Phase = 0;
  // Number of nodes per cell
  const unsigned Nnodes = 8;
  // Tolerance
  const double Tolerance = 1.E-7;

  // Coordinates
  Eigen::Vector3d coords;
  coords.setZero();

  //! Check for id = 0
  SECTION("Particle id is zero") {
    mpm::Index id = 0;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == 0);
    REQUIRE(particle->status() == true);
  }

  SECTION("Particle id is positive") {
    //! Check for id is a positive value
    mpm::Index id = std::numeric_limits<mpm::Index>::max();
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    REQUIRE(particle->id() == std::numeric_limits<mpm::Index>::max());
    REQUIRE(particle->status() == true);
  }

  //! Construct with id, coordinates and status
  SECTION("Particle with id, coordinates, and status") {
    mpm::Index id = 0;
    bool status = true;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords, status);
    REQUIRE(particle->id() == 0);
    REQUIRE(particle->status() == true);
    particle->assign_status(false);
    REQUIRE(particle->status() == false);
  }

  //! Test coordinates function
  SECTION("coordinates function is checked") {
    mpm::Index id = 0;
    // Create particle
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    //! Check for coordinates being zero
    auto coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
    REQUIRE(coordinates.size() == Dim);

    //! Check for negative value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = -1. * std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);

    //! Check for positive value of coordinates
    for (unsigned i = 0; i < coordinates.size(); ++i)
      coords(i) = std::numeric_limits<double>::max();
    particle->assign_coordinates(coords);
    coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    REQUIRE(coordinates.size() == Dim);
  }

  //! Test assign cell pointer to particle
  SECTION("Add a pointer to a cell to particle") {
    // Add particle
    mpm::Index id = 0;
    coords << 1.5, 1.5, 1.5;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check particle coordinates
    auto coordinates = particle->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));

    // Assign hexahedron shape function
    std::shared_ptr<mpm::ShapeFn<Dim>> shapefn =
        std::make_shared<mpm::HexahedronShapeFn<Dim, 8>>();

    // Create cell
    auto cell = std::make_shared<mpm::Cell<Dim>>(10, Nnodes, shapefn);
    // Add nodes
    coords << 0, 0, 0;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);

    coords << 2, 0, 0;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);

    coords << 2, 2, 0;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);

    coords << 0, 2, 0;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);

    coords << 0, 0, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node4 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(4, coords);

    coords << 2, 0, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node5 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(5, coords);

    coords << 2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node6 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(6, coords);

    coords << 0, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node7 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(7, coords);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);
    cell->add_node(4, node4);
    cell->add_node(5, node5);
    cell->add_node(6, node6);
    cell->add_node(7, node7);
    REQUIRE(cell->nnodes() == 8);

    // Initialise cell properties
    cell->initialise();

    // Check if cell is initialised
    REQUIRE(cell->is_initialised() == true);

    // Add cell to particle
    REQUIRE(cell->status() == false);
    particle->assign_cell(cell);
    REQUIRE(cell->status() == true);
    REQUIRE(particle->cell_id() == 10);

    // Check reference location
    coords << 0.5, 0.5, 0.5;
    particle->compute_reference_location();
    auto ref_coordinates = particle->reference_location();
    for (unsigned i = 0; i < ref_coordinates.size(); ++i)
      REQUIRE(ref_coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
  }

  SECTION("Check particle properties") {
    mpm::Index id = 0;
    const double Tolerance = 1.E-7;
    std::shared_ptr<mpm::ParticleBase<Dim>> particle =
        std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);

    // Check mass
    REQUIRE(particle->mass(Phase) == Approx(0.0).epsilon(Tolerance));
    double mass = 100.5;
    particle->assign_mass(Phase, mass);
    REQUIRE(particle->mass(Phase) == Approx(100.5).epsilon(Tolerance));

    // Check stress
    Eigen::Matrix<double, 6, 1> stress;
    for (unsigned i = 0; i < stress.size(); ++i) stress(i) = 1.;

    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(0.).epsilon(Tolerance));

    particle->assign_stress(Phase, stress);
    for (unsigned i = 0; i < stress.size(); ++i)
      REQUIRE(particle->stress(Phase)(i) == Approx(1.).epsilon(Tolerance));

    // Check velocity
    Eigen::VectorXd velocity;
    velocity.resize(Dim);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 17.51;

    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) == Approx(0.).epsilon(Tolerance));

    bool status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == true);
    for (unsigned i = 0; i < velocity.size(); ++i)
      REQUIRE(particle->velocity(Phase)(i) == Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of velocity
    velocity.resize(Dim * 2);
    for (unsigned i = 0; i < velocity.size(); ++i) velocity(i) = 17.51;
    status = particle->assign_velocity(Phase, velocity);
    REQUIRE(status == false);

    // Check momentum
    Eigen::VectorXd momentum;
    momentum.resize(Dim);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 17.51;

    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) == Approx(0.).epsilon(Tolerance));

    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == true);
    for (unsigned i = 0; i < momentum.size(); ++i)
      REQUIRE(particle->momentum(Phase)(i) == Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of momentum
    momentum.resize(Dim * 2);
    for (unsigned i = 0; i < momentum.size(); ++i) momentum(i) = 17.51;
    status = particle->assign_momentum(Phase, momentum);
    REQUIRE(status == false);

    // Check acceleration
    Eigen::VectorXd acceleration;
    acceleration.resize(Dim);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 17.51;

    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(0.).epsilon(Tolerance));

    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == true);
    for (unsigned i = 0; i < acceleration.size(); ++i)
      REQUIRE(particle->acceleration(Phase)(i) ==
              Approx(17.51).epsilon(Tolerance));
    // Check for incorrect dimension of acceleration
    acceleration.resize(Dim * 2);
    for (unsigned i = 0; i < acceleration.size(); ++i) acceleration(i) = 17.51;
    status = particle->assign_acceleration(Phase, acceleration);
    REQUIRE(status == false);
  }

  //! Test serialize function
  SECTION("Serialisation is checked") {
    mpm::Index id = std::numeric_limits<mpm::Index>::max();
    const double Tolerance = 1.E-7;

    // Check for negative value of coordinates
    for (unsigned i = 0; i < coords.size(); ++i) coords(i) = i;

    auto particle = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    auto buf = particle->pack();

    for (unsigned i = 0; i < coords.size(); ++i)
      std::cout << "particle 1 coords " << i << " " << particle->coordinates()(i) << std::endl;

    id = 0;
    auto particle2 = std::make_shared<mpm::Particle<Dim, Nphases>>(id, coords);
    std::cout << "particle 2 id " << particle2->id() << std::endl;
    particle2->unpack(buf);

    std::cout << "particle 1 id " << particle->id() << std::endl;
    std::cout << "particle 2 id " << particle2->id() << std::endl;

    for (unsigned i = 0; i < coords.size(); ++i)
      std::cout << "particle 2 coords " << i << " " << particle2->coordinates()(i) << std::endl;

    // Load from archive
    REQUIRE(particle2->id() == particle->id());
    auto coordinates = particle2->coordinates();
    for (unsigned i = 0; i < coordinates.size(); ++i)
      REQUIRE(coordinates(i) == Approx(coords(i)).epsilon(Tolerance));
  }
}
