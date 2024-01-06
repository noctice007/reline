#include "config.hpp"
#include <iostream>

namespace po = boost::program_options;

Config::Config(int argc, char *argv[]) {
  po::options_description desc;
  desc.add_options()("help,h", "Display help message")(
      "quite,q", "quiet mode (no output at all)")(
      "trim,t", "trim leading and trailing whitespace before comparison")(
      "file,f", po::value<std::string>(),
      "File to append the output to")("sort,s", "Sort the lines");
  po::positional_options_description pos_des;
  pos_des.add("file", 1);
  po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_des)
                .run(),
            vm);
  if (vm.count("help")) {
    std::cout << desc;
    std::exit(0);
  }
  process();
}

void Config::process() {
  po::notify(vm);
  filename = vm.count("file") > 0
                 ? std::make_optional(vm["file"].as<std::string>())
                 : std::nullopt;
  trim = vm.count("trim") > 0;
  quite = vm.count("quite") > 0;
}
