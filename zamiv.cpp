#include <iostream>

#include "cxxopts.hpp"

int main(int argc, char** argv) {
  // configure command line options
  cxxopts::Options options("zamiv", "ZAMIV's A Mediocre Image Viewer");

  // clang-format off
  options.add_options()
    ("f,fullscreen", "Start in fullscreen")
    ("h,help", "Display usage")
    ("path", "", cxxopts::value<std::string>())
    ;
  //clang-format on

  options.parse_positional({"path"});
  options.custom_help("[-fh]");
  options.positional_help("PATH");
  options.allow_unrecognised_options();
  auto result = options.parse(argc, argv);

  // check fatal options
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (!result.count("path")) {
    std::cout << "error: must specify PATH" << std::endl;
    std::cout << options.help() << std::endl;
    exit(1);
  }

  return 0;
}
