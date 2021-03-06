#include <limits>
#include <vector>

#include "Eigen/Dense"
#include "catch.hpp"
#include "json.hpp"

#include "element.h"
#include "factory.h"
#include "hexahedron_element.h"
#include "material/material.h"
#include "mesh.h"
#include "node.h"

//! \brief Check Bingham class
TEST_CASE("Bingham is checked in 2D", "[material][bingham][2D]") {
  // Tolerance
  const double Tolerance = 1.E-7;

  const unsigned Dim = 2;

  //! Check for id = 0
  SECTION("Bingham id is zero") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == 0);
  }

  SECTION("Bingham id is positive") {
    //! Check for id is a positive value
    unsigned id = std::numeric_limits<unsigned>::max();
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == std::numeric_limits<unsigned>::max());
  }

  //! Read material properties
  SECTION("Bingham check properties") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    // Check material status before assigning material property
    REQUIRE(material->status() == false);

    // Get material properties
    REQUIRE(material->property("density") ==
            Approx(std::numeric_limits<double>::max()).epsilon(Tolerance));

    // Check for property that does not exist
    REQUIRE(material->property("noproperty") ==
            Approx(std::numeric_limits<double>::max()).epsilon(Tolerance));

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->status() == true);

    // Get material properties
    REQUIRE(material->property("density") ==
            Approx(jmaterial["density"]).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with no strain rate") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    const unsigned Dim = 2;
    const unsigned Dof = 2;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 4;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);

    std::shared_ptr<mpm::Element<Dim>> element =
        Factory<mpm::Element<Dim>>::instance()->create("ED2Q4");

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, element);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0000000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(0.000e+00).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with strain rate, no yield") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    mpm::Index mesh_id = 0;
    const unsigned Dim = 2;
    const unsigned Dof = 2;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 4;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);

    std::shared_ptr<mpm::Element<Dim>> shapefn =
        Factory<mpm::Element<Dim>>::instance()->create("ED2Q4");

    node0->assign_velocity_constraint(0, 0.02);
    node0->assign_velocity_constraint(1, 0.03);
    node0->apply_velocity_constraints();

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, shapefn);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0000000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(-208333.3333333333).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(-208333.3333333333).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(0.000e+00).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with strain rate, yielded") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham2D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    mpm::Index mesh_id = 0;
    const unsigned Dim = 2;
    const unsigned Dof = 2;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 4;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);

    std::shared_ptr<mpm::Element<Dim>> shapefn =
        Factory<mpm::Element<Dim>>::instance()->create("ED2Q4");

    node0->assign_velocity_constraint(0, 2);
    node0->assign_velocity_constraint(1, 3);
    node0->apply_velocity_constraints();

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, shapefn);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0000000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(-20833765.64471337).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(-20833981.80040339).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(-540.38922505).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(0.000e+00).epsilon(Tolerance));
  }
}

TEST_CASE("Bingham is checked in 3D", "[material][bingham][3D]") {
  // Tolerance
  const double Tolerance = 1.E-7;

  const unsigned Dim = 3;

  //! Check for id = 0
  SECTION("Bingham id is zero") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == 0);
  }

  SECTION("Bingham id is positive") {
    //! Check for id is a positive value
    unsigned id = std::numeric_limits<unsigned>::max();
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == std::numeric_limits<unsigned>::max());
  }

  //! Read material properties
  SECTION("Bingham check properties") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    // Check material status before assigning material property
    REQUIRE(material->status() == false);

    // Get material properties
    REQUIRE(material->property("density") ==
            Approx(std::numeric_limits<double>::max()).epsilon(Tolerance));

    // Check for property that does not exist
    REQUIRE(material->property("noproperty") ==
            Approx(std::numeric_limits<double>::max()).epsilon(Tolerance));

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->status() == true);

    // Get material properties
    REQUIRE(material->property("density") ==
            Approx(jmaterial["density"]).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with no strain rate") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    const unsigned Dim = 3;
    const unsigned Dof = 3;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 8;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);
    coords << -2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node4 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(4, coords);
    coords << 2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node5 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(5, coords);
    coords << 2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node6 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(6, coords);
    coords << -2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node7 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(7, coords);

    std::shared_ptr<mpm::Element<Dim>> shapefn =
        Factory<mpm::Element<Dim>>::instance()->create("ED3H8");

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, shapefn);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);
    cell->add_node(4, node4);
    cell->add_node(5, node5);
    cell->add_node(6, node6);
    cell->add_node(7, node7);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0004000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(0.000e+00).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with strain rate, no yield") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    const unsigned Dim = 3;
    const unsigned Dof = 3;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 8;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);
    coords << -2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node4 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(4, coords);
    coords << 2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node5 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(5, coords);
    coords << 2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node6 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(6, coords);
    coords << -2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node7 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(7, coords);

    std::shared_ptr<mpm::Element<Dim>> shapefn =
        Factory<mpm::Element<Dim>>::instance()->create("ED3H8");

    node0->assign_velocity_constraint(0, 0.02);
    node0->assign_velocity_constraint(1, 0.03);
    node0->assign_velocity_constraint(2, 0.04);
    node0->apply_velocity_constraints();

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, shapefn);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);
    cell->add_node(4, node4);
    cell->add_node(5, node5);
    cell->add_node(6, node6);
    cell->add_node(7, node7);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0004000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(-41666.66666666666).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(-41666.66666666666).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(-41666.66666666666).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(0.000e+00).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(0.000e+00).epsilon(Tolerance));
  }

  SECTION("Bingham check stresses with strain rate, yielded") {
    unsigned id = 0;
    auto material = Factory<mpm::Material<Dim>, unsigned>::instance()->create(
        "Bingham3D", std::move(id));
    REQUIRE(material->id() == 0);

    // Initialise material
    Json jmaterial;
    jmaterial["density"] = 1000.;
    jmaterial["youngs_modulus"] = 1.0E+7;
    jmaterial["poisson_ratio"] = 0.3;
    jmaterial["tau0"] = 771.8;
    jmaterial["mu"] = 0.0451;
    jmaterial["critical_shear_rate"] = 0.2;

    material->properties(jmaterial);

    // Check material status after assigning material property
    REQUIRE(material->property_handle() == true);

    // Add particle
    mpm::Index pid = 0;
    Eigen::Matrix<double, Dim, 1> coords;
    coords << 0.5, 0.5, 0.5;
    auto particle = std::make_shared<mpm::Particle<Dim, 1>>(pid, coords);

    // Coordinates of nodes for the cell
    mpm::Index cell_id = 0;
    const unsigned Dim = 3;
    const unsigned Dof = 3;
    const unsigned Nphases = 1;
    const unsigned Nnodes = 8;
    const unsigned phase = 0;
    const double dt = 1;

    coords << -2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node0 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(0, coords);
    coords << 2, 2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node1 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(1, coords);
    coords << 2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node2 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(2, coords);
    coords << -2, 2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node3 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(3, coords);
    coords << -2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node4 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(4, coords);
    coords << 2, -2, -2;
    std::shared_ptr<mpm::NodeBase<Dim>> node5 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(5, coords);
    coords << 2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node6 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(6, coords);
    coords << -2, -2, 2;
    std::shared_ptr<mpm::NodeBase<Dim>> node7 =
        std::make_shared<mpm::Node<Dim, Dof, Nphases>>(7, coords);

    std::shared_ptr<mpm::Element<Dim>> shapefn =
        Factory<mpm::Element<Dim>>::instance()->create("ED3H8");

    node0->assign_velocity_constraint(0, 2);
    node0->assign_velocity_constraint(1, 3);
    node0->assign_velocity_constraint(2, 4);
    node0->apply_velocity_constraints();

    auto cell = std::make_shared<mpm::Cell<Dim>>(cell_id, Nnodes, shapefn);

    cell->add_node(0, node0);
    cell->add_node(1, node1);
    cell->add_node(2, node2);
    cell->add_node(3, node3);
    cell->add_node(4, node4);
    cell->add_node(5, node5);
    cell->add_node(6, node6);
    cell->add_node(7, node7);

    // Initialise cell
    REQUIRE(cell->initialise() == true);
    // Check if cell is initialised, after addition of nodes
    REQUIRE(cell->is_initialised() == true);

    particle->assign_cell(cell);
    particle->assign_material(material);
    particle->compute_shapefn();
    particle->compute_strain(phase, dt);

    // Initialise dstrain
    mpm::Material<Dim>::Vector6d dstrain;
    dstrain(0) = -0.0010000;
    dstrain(1) = 0.0005000;
    dstrain(2) = 0.0004000;
    dstrain(3) = 0.0000000;
    dstrain(4) = 0.0000000;
    dstrain(5) = 0.0000000;

    // Compute updated stress
    mpm::Material<Dim>::Vector6d stress;
    stress.setZero();
    auto check_stress =
        material->compute_stress(stress, dstrain, particle.get());

    // Check stressees
    REQUIRE(check_stress.size() == 6);
    REQUIRE(check_stress(0) == Approx(-4167039.740848102).epsilon(Tolerance));
    REQUIRE(check_stress(1) == Approx(-4167002.433429959).epsilon(Tolerance));
    REQUIRE(check_stress(2) == Approx(-4165920.518303796).epsilon(Tolerance));
    REQUIRE(check_stress(3) == Approx(-391.727890507).epsilon(Tolerance));
    REQUIRE(check_stress(4) == Approx(55.961127215).epsilon(Tolerance));
    REQUIRE(check_stress(5) == Approx(-186.537090718).epsilon(Tolerance));
  }
}
