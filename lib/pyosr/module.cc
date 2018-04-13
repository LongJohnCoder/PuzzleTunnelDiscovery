#include <osr/osr_state.h>
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
	m.def("distance", &osr::distance,
	      "Calculate the distance between two unit states");
	m.def("differential", &osr::differential,
	      "Calculate the action from one unit state to another", py::call_guard<py::gil_scoped_release>());
	m.def("get_permutation_to_world", &osr::get_permutation_to_world);
	using osr::UnitWorld;
	py::class_<UnitWorld>(m, "UnitWorld")
		.def(py::init<>())
		.def("setupFrom", &UnitWorld::copyFrom)
		.def("loadModelFromFile", &UnitWorld::loadModelFromFile)
		.def("loadRobotFromFile", &UnitWorld::loadRobotFromFile)
		.def("scaleToUnit", &UnitWorld::scaleToUnit)
		.def("angleModel", &UnitWorld::angleModel)
		.def("set_perturbation", &UnitWorld::setPerturbation)
		.def_property("state", &UnitWorld::getRobotState, &UnitWorld::setRobotState)
		.def("is_valid_state", &UnitWorld::isValid, py::call_guard<py::gil_scoped_release>())
		.def("is_disentangled", &UnitWorld::isDisentangled, py::call_guard<py::gil_scoped_release>())
		.def("transit_state", &UnitWorld::transitState, py::call_guard<py::gil_scoped_release>())
		.def("transit_state_to", &UnitWorld::transitStateTo, py::call_guard<py::gil_scoped_release>())
		.def("transit_state_by", &UnitWorld::transitStateBy, py::call_guard<py::gil_scoped_release>())
		.def("translate_to_unit_state", &UnitWorld::translateToUnitState, py::call_guard<py::gil_scoped_release>())
		.def("translate_from_unit_state", &UnitWorld::translateFromUnitState, py::call_guard<py::gil_scoped_release>())
		.def_property_readonly("scene_matrix", &UnitWorld::getSceneMatrix)
		.def_property_readonly("robot_matrix", &UnitWorld::getRobotMatrix);
#if GPU_ENABLED
	using osr::Renderer;
	py::class_<Renderer, UnitWorld>(m, "Renderer")
		.def(py::init<>())
		.def("setup", &Renderer::setup)
		.def("setupFrom", &Renderer::setupFrom)
		.def("teardown", &Renderer::teardown)
		// .def("loadModelFromFile", &Renderer::loadModelFromFile)
		// .def("loadRobotFromFile", &Renderer::loadRobotFromFile)
		.def("render_depth_to_buffer", &Renderer::render_depth_to_buffer)
		.def("render_mvdepth_to_buffer", &Renderer::render_mvdepth_to_buffer)
		.def("render_mvrgbd", &Renderer::render_mvrgbd, py::call_guard<py::gil_scoped_release>())
		.def_readwrite("pbufferWidth", &Renderer::pbufferWidth)
		.def_readwrite("pbufferHeight", &Renderer::pbufferHeight)
		.def_readwrite("default_depth", &Renderer::default_depth)
		.def_readwrite("mvrgb", &Renderer::mvrgb)
		.def_readwrite("mvdepth", &Renderer::mvdepth)
		.def_readwrite("views", &Renderer::views)
		.def_readwrite("avi", &Renderer::avi);
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
		     py::arg("max_steps") = -1)
		.def_readwrite("verify_magnitude", &GTGenerator::verify_magnitude)
		.def_readwrite("gamma", &GTGenerator::gamma)
		.def_readwrite("rl_stepping_size", &GTGenerator::rl_stepping_size)
		;
	m.def("interpolate", &osr::interpolate,
	      "Interpolate between two SE3 states");
	return m.ptr();
}
