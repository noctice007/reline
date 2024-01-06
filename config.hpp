#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <boost/program_options.hpp>

struct Config {
  explicit Config(int argc, char *argv[]);
  void process();
  std::optional<std::string> filename;
  boost::program_options::variables_map vm;
  bool quite;
  bool trim;
};

#endif
