#include "tee.hpp"
#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/program_options.hpp>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

namespace po = boost::program_options;
namespace ranges = std::ranges;
namespace views = ranges::views;

struct Config {
  explicit Config(int argc, char *argv[]) {
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
  void process() {
    po::notify(vm);
    filename = vm.count("file") > 0
                   ? std::make_optional(vm["file"].as<std::string>())
                   : std::nullopt;
    trim = vm.count("trim") > 0;
    quite = vm.count("quite") > 0;
  }
  std::optional<std::string> filename;
  po::variables_map vm;
  bool quite;
  bool trim;
};

std::vector<std::string> input(std::istream &in) noexcept {
  std::vector<std::string> lines;
  for (std::string input; std::getline(in, input); lines.push_back(input)) {
  }
  return lines;
}

int main(int argc, char *argv[]) {
  try {
    std::ios::sync_with_stdio(false);

    const Config config(argc, argv);
    std::unordered_set<std::string> seen;
    std::unique_ptr<std::ostream> out;
    std::fstream file;
    boost::iostreams::stream<boost::iostreams::null_sink> nullstream(
        boost::iostreams::null_sink{});

    if (config.filename) {
      file.open(config.filename.value(),
                std::ios::in | std::ios::out | std::ios::app);
      std::vector<std::string> file_lines;
      file_lines = input(file);
      seen.insert(file_lines.begin(), file_lines.end());
    }
    std::ostream &file_ostream = file;
    out.reset(new teestream(
        config.filename ? file_ostream
                        : nullstream, // redirect to null if file not specified
        config.quite ? nullstream
                     : std::cout) // redirect to null if in quite mode
    );

    auto trim = views::drop_while([](char ch) { return std::isspace(ch); });
    for (std::string line; std::getline(std::cin, line);) {
      if (config.trim) {
        auto leading_trim =
            line | trim | views::reverse | trim | views::reverse;
        line.assign(leading_trim.begin(), leading_trim.end());
      }
      if (auto line_ptr = seen.insert(std::move(line)); line_ptr.second) {
        *out << *line_ptr.first << '\n';
      }
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}
