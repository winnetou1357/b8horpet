#ifndef RPS_OPTIONS_HXX
#define RPS_OPTIONS_HXX

namespace rps {

  struct Options
  {
    enum class Mode : char {
      Undefined,
      Server,
      Client
    };

    Mode        mode;
    std::string host;
    short       port;

    Options()
      : mode(Mode::Undefined)
      , host("any")
      , port(0)
    { }
  };

} // namespace rps

#endif // RPS_OPTIONS_HXX
