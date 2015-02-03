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

void HandleCommandLine(Options &opts, int argc, char **argv);
