#include <pybind11/embed.h>  // everything needed for embedding
#include <pybind11/pybind11.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace py = pybind11;
using namespace py::literals;

void init_python_interpreter() {
  std::filesystem::path this_file_path(__FILE__);
  auto python_module_path = this_file_path.parent_path();
  char *current_python_path = getenv("PYTHONPATH");
  std::string new_python_path;
  if (current_python_path) {
    new_python_path = current_python_path;
#ifdef WIN32
    new_python_path += ";";
#else
    new_python_path += ":";
#endif
  }
  new_python_path += python_module_path.string();
#ifdef WIN32
  system(
      "python -c \"import sys;"
      "print(sys.prefix)\" > tmp.txt");
  std::ifstream fin("tmp.txt");
  std::string python_home_path;
  std::getline(fin, python_home_path);
  fin.close();
  std::string python_home_env = "PYTHONHOME=" + python_home_path;
  _putenv(python_home_env.c_str());
  std::string python_path = "PYTHONPATH=" + python_home_path +
                            "\\Lib\\site-packages;" + new_python_path;
  _putenv(python_path.c_str());
  std::cout << python_home_env << std::endl;
  std::cout << python_path << std::endl;
#else
  setenv("PYTHONPATH", new_python_path.c_str(), 1);
#endif
}

int add(int i, int j) { return i + j; }

PYBIND11_EMBEDDED_MODULE(fast_calc, m) {
  m.doc() = "example embedded module";  // optional module docstring

  m.def("add", &add, "A function that adds two numbers");
}

int main() {
  init_python_interpreter();
  // start the interpreter and keep it alive
  py::scoped_interpreter guard{};

  // Hello, World!
  auto kwargs = py::dict("name"_a = "World", "number"_a = 42);
  auto message = "Hello, {name}! The answer is {number}"_s.format(**kwargs);
  py::print(message);

  // sys.path
  py::module sys = py::module::import("sys");
  py::print(sys.attr("path"));

  // adding embedded modules
  auto fast_calc = py::module::import("fast_calc");
  auto result = fast_calc.attr("add")(1, 2).cast<int>();
  assert(result == 3);

  // importing modules
  auto calc = py::module::import("calc");
  result = calc.attr("add")(1, 2).cast<int>();
  assert(result == 3);

  auto mul = py::module::import("package.mul");
  result = mul.attr("mul")(3, 5).cast<int>();
  assert(result == 15);

  auto hello = py::module::import("package.nested");
  auto greetings = py::reinterpret_steal<py::function>(hello.attr("greetings"));
  greetings();
  return 0;
}
