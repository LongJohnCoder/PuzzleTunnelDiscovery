/**
 * SPDX-FileCopyrightText: Copyright © 2020 The University of Texas at Austin
 * SPDX-FileContributor: Xinya Zhang <xinyazhang@utexas.edu>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <osr/osr_state.h>
#include <osr/osr_util.h>
#include <osr/unit_world.h>
#include <osr/osr_render.h>
#include <osr/osr_init.h>
#include <osr/gtgenerator.h>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <iostream>
#include <stdint.h>
namespace py = pybind11;

#if GPU_ENABLED
namespace {
uintptr_t create_display_wrapper(int device_idx = 0)
{
	EGLDisplay dpy = osr::create_display(device_idx);
	std::cerr << "dpy: " << dpy << std::endl;
	std::cerr << "sizeof dpy: " << sizeof(dpy) << std::endl;
	return (uintptr_t)dpy;
}

void create_gl_context_wrapper(uintptr_t dpy)
{
	osr::create_gl_context((EGLDisplay)dpy);
}

}
#endif // GPU_ENABLED

// For older pybind11
// Replace this with PYBIND11_MODULE(osr, m) after pybind11 v2.2.0
PYBIND11_PLUGIN(pyosr) {
	py::module m("pyosr", "Off-Screen Rendering");
#if GPU_ENABLED
	m.def("init", &osr::init, "Initialize OSR");
#if 1
	m.def("create_display",
	      &osr::create_display,
	      "Create EGL Display",
	      py::arg("device_idx") = 0);
	m.def("create_gl_context", &osr::create_gl_context,
	      "Create OpenGL 3.3 Core Profile Context",
	      py::arg("dpy"),
	      py::arg("share_context") = EGL_NO_CONTEXT
	      );
#else
	m.def("create_display",
	      &create_display_wrapper,
	      "Create EGL Display",
	      py::arg("device_idx") = 0);
	m.def("create_gl_context", &create_gl_context_wrapper,
	      "Create OpenGL 3.3 Core Profile Context");
#endif
	m.def("shutdown", &osr::shutdown,
	      "Close internally opened FDs. "
	      "User is responsible to free OpenGL/EGL resources");
#endif // GPU_ENABLED
#if 0
	m.def("transit", &osr::transitState,
	      "State Transition, given action and corresponding magnitude",
	      py::arg("state"),
	      py::arg("action"),
	      py::arg("mag"));
#endif
#if 0
	py::class_<Pet>(m, "Pet")
		.def(py::init<const std::string &>())
		.def("setName", &Pet::setName)
		.def("getName", &Pet::getName);
#endif
	m.def("get_identity_state_vector", &osr::get_identity_state_vector,
	      "Return the identity state");
	m.def("decompose_2", &osr::decompose_2,
	      "Decompose the SE(3) state vector into a translation vector and a 3x3 rotation matrix");
	m.def("decompose_3", &osr::decompose_3,
	      "Decompose the SE(3) state vector into a translation vector and angle-axis");
	m.def("compose_from_angleaxis", &osr::compose_from_angleaxis,
	      "Compose the SE(3) state vector from a translation vector and angle axis representation");
	m.def("distance", &osr::distance,
	      "Calculate the distance between two unit states");
	m.def("multi_distance", &osr::multi_distance,
	      "Calculate distances of one-vs-many.");
	m.def("differential", &osr::differential,
	      "Calculate the action from one unit state to another", py::call_guard<py::gil_scoped_release>());
	m.def("multi_differential", &osr::multi_differential,
	      py::arg("from"),
	      py::arg("tos"),
	      py::arg("with_se3") = false,
	      "Calculate the action from one unit state to another group", py::call_guard<py::gil_scoped_release>());
	m.def("apply", &osr::apply,
	      "Apply a continuous action to one state vector", py::call_guard<py::gil_scoped_release>());
	m.def("get_permutation_to_world", &osr::get_permutation_to_world);
	m.def("extract_rotation_matrix", &osr::extract_rotation_matrix);
	m.def("save_obj_1", &osr::saveOBJ1);
	m.def("save_obj_2", &osr::saveOBJ2,
	      py::arg("V"),
	      py::arg("F"),
	      py::arg("CN"),
	      py::arg("FN"),
	      py::arg("TC"),
	      py::arg("FTC"),
	      py::arg("filename")
	     );
	m.def("save_ply_2", &osr::savePLY2,
	      py::arg("V"),
	      py::arg("F"),
	      py::arg("N"),
	      py::arg("UV"),
	      py::arg("filename")
	     );
	m.def("load_obj_1", &osr::loadOBJ1);
	m.def("mesh_bool", &osr::meshBool, py::call_guard<py::gil_scoped_release>());
	m.attr("STATE_DIMENSION") = py::int_(osr::kStateDimension);
	m.attr("MESH_BOOL_UNION") = py::int_(osr::MESH_BOOL_UNION);
	m.attr("MESH_BOOL_INTERSECT") = py::int_(osr::MESH_BOOL_INTERSECT);
	m.attr("MESH_BOOL_MINUS") = py::int_(osr::MESH_BOOL_MINUS);
	m.attr("MESH_BOOL_XOR") = py::int_(osr::MESH_BOOL_XOR);
	m.attr("MESH_BOOL_RESOLVE") = py::int_(osr::MESH_BOOL_RESOLVE);
	m.def("tritri_cop", &osr::tritriCop);
	using osr::UnitWorld;
	py::class_<UnitWorld>(m, "UnitWorld")
		.def(py::init<>())
		.def("setupFrom", &UnitWorld::copyFrom)
		.def("loadModelFromFile", &UnitWorld::loadModelFromFile)
		.def("loadRobotFromFile", &UnitWorld::loadRobotFromFile)
		.def("enforceRobotCenter", &UnitWorld::enforceRobotCenter)
		.def("scaleToUnit", &UnitWorld::scaleToUnit)
		.def("angleModel", &UnitWorld::angleModel)
		.def("set_perturbation", &UnitWorld::setPerturbation)
		.def_property_readonly("perturbation", &UnitWorld::getPerturbation)
		.def_property("state", &UnitWorld::getRobotState, &UnitWorld::setRobotState)
		.def("is_valid_state", &UnitWorld::isValid, py::call_guard<py::gil_scoped_release>())
		.def("is_disentangled", &UnitWorld::isDisentangled, py::call_guard<py::gil_scoped_release>())
		.def("transit_state", &UnitWorld::transitState, py::call_guard<py::gil_scoped_release>())
		.def("transit_state_to", &UnitWorld::transitStateTo,
		     py::arg("from"),
		     py::arg("to"),
		     py::arg("verify_delta"),
		     py::call_guard<py::gil_scoped_release>())
		.def("transit_state_to_with_contact", &UnitWorld::transitStateToWithContact,
		     py::arg("from"),
		     py::arg("to"),
		     py::arg("verify_delta"),
		     py::call_guard<py::gil_scoped_release>())
		.def("is_valid_transition", &UnitWorld::isValidTransition,
		     py::arg("from"),
		     py::arg("to"),
		     py::arg("initial_verify_delta"),
		     py::call_guard<py::gil_scoped_release>())
		.def("transit_state_by", &UnitWorld::transitStateBy, py::call_guard<py::gil_scoped_release>())
		.def("translate_to_unit_state", &UnitWorld::translateToUnitState, py::call_guard<py::gil_scoped_release>())
		.def("translate_from_unit_state", &UnitWorld::translateFromUnitState, py::call_guard<py::gil_scoped_release>())
		.def("translate_unit_to_ompl", &UnitWorld::translateUnitStateToOMPLState,
		     py::arg("Q"),
		     py::arg("to_angle_axis") = false,
		     py::call_guard<py::gil_scoped_release>())
		.def("translate_ompl_to_unit", &UnitWorld::translateOMPLStateToUnitState)
		.def("translate_vanilla_to_ompl", &UnitWorld::translateVanillaStateToOMPLState)
		.def("translate_ompl_to_vanilla", &UnitWorld::translateOMPLStateToVanillaState)
		.def("translate_vanilla_to_unit", &UnitWorld::translateVanillaStateToUnitState)
		.def("translate_vanilla_pts_to_unit", &UnitWorld::translateVanillaPointsToUnitPoints)
		.def("calculate_visibility_matrix", &UnitWorld::calculateVisibilityMatrix, py::call_guard<py::gil_scoped_release>())
		.def("calculate_visibility_matrix2", &UnitWorld::calculateVisibilityMatrix2,
				py::arg("qs0"),
				py::arg("qs0_are_unit_states"),
				py::arg("qs1"),
				py::arg("qs1_are_unit_states"),
				py::arg("verify_magnitude"),
				py::arg("enable_mt") = true,
				py::call_guard<py::gil_scoped_release>())
		.def("calculate_visibility_pair", &UnitWorld::calculateVisibilityPair,
				py::arg("qs0"),
				py::arg("qs0_are_unit_states"),
				py::arg("qs1"),
				py::arg("qs1_are_unit_states"),
				py::arg("verify_magnitude"),
				py::arg("enable_mt") = true,
				py::call_guard<py::gil_scoped_release>())
#if PYOSR_HAS_MESHBOOL
		.def("intersection_region_surface_areas", &UnitWorld::intersectionRegionSurfaceAreas, py::call_guard<py::gil_scoped_release>())
		.def("intersecting_geometry", &UnitWorld::intersectingGeometry, py::call_guard<py::gil_scoped_release>())
#endif
		.def("intersecting_to_robot_surface", &UnitWorld::intersectingToRobotSurface, py::call_guard<py::gil_scoped_release>())
		.def("intersecting_to_model_surface", &UnitWorld::intersectingToModelSurface, py::call_guard<py::gil_scoped_release>())
		.def("get_robot_geometry", &UnitWorld::getRobotGeometry, py::call_guard<py::gil_scoped_release>())
		.def("get_scene_geometry", &UnitWorld::getSceneGeometry, py::call_guard<py::gil_scoped_release>())
		.def("intersecting_segments", &UnitWorld::intersectingSegments, py::call_guard<py::gil_scoped_release>())
		.def("robot_face_normals_from_indices", py::overload_cast<const Eigen::Matrix<int, -1, 1>&>(&UnitWorld::getRobotFaceNormalsFromIndices), py::call_guard<py::gil_scoped_release>())
		.def("scene_face_normals_from_indices", py::overload_cast<const Eigen::Matrix<int, -1, 1>&>(&UnitWorld::getSceneFaceNormalsFromIndices), py::call_guard<py::gil_scoped_release>())
		.def("robot_face_normals_from_index_pairs", py::overload_cast<const Eigen::Matrix<int, -1, 2>&>(&UnitWorld::getRobotFaceNormalsFromIndices), py::call_guard<py::gil_scoped_release>())
		.def("scene_face_normals_from_index_pairs", py::overload_cast<const Eigen::Matrix<int, -1, 2>&>(&UnitWorld::getSceneFaceNormalsFromIndices), py::call_guard<py::gil_scoped_release>())
		.def("force_direction_from_intersecting_segments", &UnitWorld::forceDirectionFromIntersectingSegments, py::call_guard<py::gil_scoped_release>())
		.def("push_robot", &UnitWorld::pushRobot, py::call_guard<py::gil_scoped_release>())
		.def("sample_over_primitive", &UnitWorld::sampleOverPrimitive,
		     py::arg("geo"),
		     py::arg("prim"),
		     py::arg("return_unit") = true,
		     py::call_guard<py::gil_scoped_release>())
		.def("uv_to_surface", &UnitWorld::uvToSurface,
		     py::arg("geo"),
		     py::arg("prim"),
		     py::arg("uv"),
		     py::arg("return_unit") = true,
		     py::call_guard<py::gil_scoped_release>())
		.def("sample_free_configuration", &UnitWorld::sampleFreeConfiguration,
		     py::arg("rob_surface_point"),
		     py::arg("rob_surface_normal"),
		     py::arg("env_surface_point"),
		     py::arg("env_surface_normal"),
		     py::arg("margin"),
		     py::arg("max_trials") = -1,
		     py::call_guard<py::gil_scoped_release>())
		.def("enum_free_configuration", &UnitWorld::enumFreeConfiguration,
		     py::arg("rob_surface_point"),
		     py::arg("rob_surface_normal"),
		     py::arg("env_surface_point"),
		     py::arg("env_surface_normal"),
		     py::arg("margin"),
		     py::arg("denominator"),
		     py::arg("only_median") = false,
		     py::call_guard<py::gil_scoped_release>())
		.def("enum_2drot_free_configuration", &UnitWorld::enum2DRotationFreeConfiguration,
		     py::arg("rob_surface_point"),
		     py::arg("rob_surface_normal"),
		     py::arg("rob_prim_id"),
		     py::arg("env_surface_point"),
		     py::arg("env_surface_normal"),
		     py::arg("env_prim_id"),
		     py::arg("margin"),
		     py::arg("altitude_divider"),
		     py::arg("azimuth_divider"),
		     py::arg("return_all") = false,
		     py::arg("enable_mt") = true,
		     py::call_guard<py::gil_scoped_release>())
		.def("get_ompl_center", &UnitWorld::getOMPLCenter,
		     py::arg("geo") = UnitWorld::GEO_ROB)
		.def("kinetic_energy_distance", &UnitWorld::kineticEnergyDistance)
		.def("multi_kinetic_energy_distance", &UnitWorld::multiKineticEnergyDistance)
		.def_readonly_static("GEO_ENV", &UnitWorld::GEO_ENV)
		.def_readonly_static("GEO_ROB", &UnitWorld::GEO_ROB)
		.def_property("recommended_cres", &UnitWorld::getRecommendedCres, &UnitWorld::setRecommendedCres)
		.def_property_readonly("scene_scale", &UnitWorld::getSceneScale)
		.def_property_readonly("scene_matrix", &UnitWorld::getSceneMatrix)
		.def_property_readonly("robot_matrix", &UnitWorld::getRobotMatrix);
#if GPU_ENABLED
	using osr::Renderer;
	py::class_<Renderer, UnitWorld>(m, "Renderer")
		.def(py::init<>())
		.def("setup", &Renderer::setup)
		.def("setupFrom", &Renderer::setupFrom)
		.def("loadRobotTextureImage", &Renderer::loadRobotTextureImage)
		.def("teardown", &Renderer::teardown)
		// .def("loadModelFromFile", &Renderer::loadModelFromFile)
		// .def("loadRobotFromFile", &Renderer::loadRobotFromFile)
		.def("render_depth_to_buffer", &Renderer::render_depth_to_buffer)
		.def("render_mvdepth_to_buffer", &Renderer::render_mvdepth_to_buffer)
		.def("render_mvrgbd", &Renderer::render_mvrgbd,
		     py::arg("flags") = 0,
		     py::call_guard<py::gil_scoped_release>())
		.def("add_barycentric", &Renderer::addBarycentric,
		     py::arg("F"),
		     py::arg("V"),
		     py::arg("target"),
		     py::arg("weight") = 1.0,
		     py::call_guard<py::gil_scoped_release>())
		.def("clear_barycentric", &Renderer::clearBarycentric,
		     py::call_guard<py::gil_scoped_release>())
		.def("render_barycentric", &Renderer::renderBarycentric,
		     py::arg("target"),
		     py::arg("res"),
		     py::arg("svg_fn") = std::string(),
		     py::call_guard<py::gil_scoped_release>())
		.def_readonly_static("NO_SCENE_RENDERING", &Renderer::NO_SCENE_RENDERING)
		.def_readonly_static("NO_ROBOT_RENDERING", &Renderer::NO_ROBOT_RENDERING)
		.def_readonly_static("HAS_NTR_RENDERING", &Renderer::HAS_NTR_RENDERING)
		.def_readonly_static("UV_MAPPINNG_RENDERING", &Renderer::UV_MAPPINNG_RENDERING)
		.def_readonly_static("NORMAL_RENDERING", &Renderer::NORMAL_RENDERING)
		.def_readonly_static("UV_FEEDBACK", &Renderer::UV_FEEDBACK)
		.def_readonly_static("BARY_RENDERING_ROBOT", &Renderer::BARY_RENDERING_ROBOT)
		.def_readonly_static("BARY_RENDERING_SCENE", &Renderer::BARY_RENDERING_SCENE)
		.def_readwrite("pbufferWidth", &Renderer::pbufferWidth)
		.def_readwrite("pbufferHeight", &Renderer::pbufferHeight)
		.def_readwrite("default_depth", &Renderer::default_depth)
		.def_readwrite("mvrgb", &Renderer::mvrgb)
		.def_readwrite("mvdepth", &Renderer::mvdepth)
		.def_readwrite("mvuv", &Renderer::mvuv)
		.def_readwrite("mvpid", &Renderer::mvpid)
		.def_readwrite("mvnormal", &Renderer::mvnormal)
		.def_readwrite("views", &Renderer::views)
		.def_readwrite("avi", &Renderer::avi)
		.def_readwrite("flat_surface", &Renderer::flat_surface)
		.def_readwrite("light_position", &Renderer::light_position)
		.def_property("final_scaling", &Renderer::getFinalScaling, &Renderer::setFinalScaling)
		// .def_property("uv_feedback", &Renderer::getUVFeedback, &Renderer::setUVFeedback)
		;
#endif // GPU_ENABLED
	using osr::GTGenerator;
	py::class_<GTGenerator>(m, "GTGenerator")
		.def(py::init<UnitWorld&>())
		.def("load_roadmap_file", &GTGenerator::loadRoadMapFile)
		.def("save_verified_roadmap_file", &GTGenerator::saveVerifiedRoadMapFile)
		.def("init_knn", &GTGenerator::initKNN)
		.def("init_knn_in_batch", &GTGenerator::initKNNInBatch)
		.def("init_gt", &GTGenerator::initGT)
		.def("install_gtdata", &GTGenerator::installGTData)
		.def("extract_gtdata", &GTGenerator::extractGTData)
		.def("generate_gt_path", &GTGenerator::generateGTPath)
		.def("cast_path_to_cont_actions_in_UW", &GTGenerator::castPathToContActionsInUW,
		     py::arg("path"),
		     py::arg("path_is_verified") = false
		    )
		.def("project_trajectory", &GTGenerator::projectTrajectory,
		     py::arg("from"),
		     py::arg("to"),
		     py::arg("max_steps") = -1,
		     py::arg("in_unit") = true
		     )
		.def_readwrite("verify_magnitude", &GTGenerator::verify_magnitude)
		.def_readwrite("gamma", &GTGenerator::gamma)
		.def_readwrite("rl_stepping_size", &GTGenerator::rl_stepping_size)
		;
	m.def("interpolate", &osr::interpolate,
	      "Interpolate between two SE3 states");
	m.def("path_metrics", &osr::path_metrics,
	      "Distances of each node on the path to the beginning of this path");
	m.def("path_interpolate", &osr::path_interpolate,
	      "Interpolate over the path");
	return m.ptr();
}
