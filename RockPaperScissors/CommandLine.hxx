#ifndef RPS_COMMANDLINE_HXX
#define RPS_COMMANDLINE_HXX

namespace rps {
  struct Options;
}

namespace rps {
namespace commandline {

  void Handle(rps::Options &opts, int argc, char **argv);

} // namespace rps
} // namespace commandline

#endif // RPS_COMMANDLINE_HXX
