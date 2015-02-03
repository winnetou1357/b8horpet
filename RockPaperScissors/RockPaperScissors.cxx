#include <string>
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>
#include <set>

#include <boost/program_options.hpp>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/system/error_code.hpp>

#include "RockPaperScissorsConfig.hxx"

namespace p2p
{
  using namespace boost::asio;

  /**
   * Abstract session base class.
   */
  class Session
  {
  public:
    enum class Mode : char {
      Undefined,
      Server,
      Client
    };
    
    class InvalidMode : public std::exception
    {
    public:
      const char* what() const throw() override {
        return "p2p::Session::InvalidMode";
      }
    };
  
    Session (io_service& ioService, Mode mode, const std::string& host, short port)
      : mIoService(ioService)
      , mResolver(ioService)
      , mSocket(ioService)
      , mMode(mode)
      , mHost(host)
      , mPort(port)
    {
      switch (mMode)
      {
        case Mode::Server:
          mHost = "";
          break;
        case Mode::Client:
          break;
        default:
          throw InvalidMode();
          break;
      }
    }

    ~Session ()
    {
      if (mSocket.is_open()) {
        mSocket.close ();
      }
    }

    void run ()
    {
      try {
        ip::tcp::resolver::iterator iter = mResolver.async_resolve(ip::tcp::resolver::query(ip::tcp::v4(), mHost, std::to_string(mPort)), boost::asio::use_future).get();
        if (iter == ip::tcp::resolver::iterator()) {
          std::cout << "Could not resolve hostname " << mHost << std::endl;
        }
        else {
          std::for_each(iter, ip::tcp::resolver::iterator(), [&](const ip::tcp::endpoint& endpoint) {
            std::cout << "Resolved endpoint " << endpoint << std::endl;
          });

          connect(iter);
        }
      }
      catch (std::system_error& ex) {
        std::cout << "Error while resolving hostname " << ex.what() << std::endl;
      }
    }

  protected:
    void connect(const ip::tcp::resolver::iterator& endpointIterator)
    {
      try {
        if (mMode == Mode::Client) {
          boost::asio::async_connect(mSocket, endpointIterator, boost::asio::use_future).get();
        } else if (mMode == Mode::Server) {
          ip::tcp::acceptor acceptor(mIoService, ip::tcp::endpoint(ip::tcp::v4(), mPort));
 
          std::cout << "Waiting for connection on port " << mPort << std::endl;
          acceptor.async_accept(mSocket, boost::asio::use_future).get();
        } else {
          throw InvalidMode();
        }
        
        char outData[] = "Now Im here!";
        std::future<std::size_t> write = boost::asio::async_write(mSocket, boost::asio::buffer(outData, sizeof(outData)), boost::asio::use_future);
        
        char inData[1024];
        std::fill(inData, inData + sizeof(inData), '\0');
        std::future<std::size_t> read = mSocket.async_read_some(boost::asio::buffer(inData, sizeof(inData)), boost::asio::use_future);
                
        std::cout << "Wrote " << write.get() << " bytes" << std::endl;
        std::cout << "Read " << read.get() << " bytes: " << inData << std::endl;
      }
      catch (const std::system_error& ex) {
        std::cout << "Error while connecting " << ex.what() << std::endl;
      }
    }
    
  private:
    io_service& mIoService;

    ip::tcp::resolver mResolver;
    ip::tcp::socket   mSocket;
	
    Mode        mMode;
    std::string mHost;
    short       mPort;
    
  };
}

struct Options
{
  p2p::Session::Mode  mode;
  std::string         host;
  short               port;

  Options()
    : mode(p2p::Session::Mode::Undefined)
    , host("")
    , port(0)
  { }
};

void HandleCommandLine(Options &opts, int argc, char **argv)
{
  namespace po = boost::program_options;

  std::set<std::string> allowedSubcommands{"server", "client"};
  std::ostringstream allowedSubcommandsOutput;
  for (const std::string &sc : allowedSubcommands) {
    allowedSubcommandsOutput << sc << ", ";
  }
  std::string allowedSubcommandsAsString = allowedSubcommandsOutput.str();
  allowedSubcommandsAsString.pop_back();  // trim trailing ", "
  allowedSubcommandsAsString.pop_back();
  
  po::positional_options_description pos;
  pos.add("subcommand", 1)
     .add("subargs", -1)
  ;

  po::options_description desc("USAGE: RockPaperScissors [global options] [subcommand and suboptions]");
  desc.add_options()
    ("help,h", "produce help message")
    ("subcommand", po::value<std::string>()->required(), allowedSubcommandsAsString.c_str())
    ("subargs", po::value<std::vector<std::string>>(), "options for subcommand - use '--help subcommand' for details")
  ;

  try
  {
    po::variables_map vm;
    po::parsed_options parsed
      = po::command_line_parser(argc, argv)
        .options(desc)
        .positional(pos)
        .allow_unregistered()
        .run();
    po::store(parsed, vm);
    
    if (vm.empty() || vm.count("help"))
    {
      std::cerr << desc << std::endl;
      exit(1);
    }

    po::notify(vm);
    
    std::string subcommand = vm["subcommand"].as<std::string>();
    
    if (subcommand == "server")
    {
      po::positional_options_description serverPos;
      serverPos.add("port", 1);
      
      po::options_description serverDesc;
      serverDesc.add_options()
        ("port,p", po::value<short>()->required(), "network port to communicate on with the remote host")
      ;

      if (vm.count("help")) {
        std::cerr << serverDesc << std::endl;
        exit(1);
      }

      std::vector<std::string> serverOpts = po::collect_unrecognized(parsed.options, po::include_positional);
      serverOpts.erase(serverOpts.begin());
      po::store(po::command_line_parser(serverOpts).options(serverDesc).positional(serverPos).run(), vm);
      po::notify(vm);

      opts.mode = p2p::Session::Mode::Server;
      opts.host = "any";
      opts.port = vm.count("port") ? vm["port"].as<short>() : 0;
    }
    else if (subcommand == "client")
    {
      po::positional_options_description clientPos;
      clientPos.add("server", 1);
      clientPos.add("port", 1);
      
      po::options_description clientDesc;
      clientDesc.add_options()
        ("server,s", po::value<std::string>()->required(), "name or IP address of remote host")
        ("port,p", po::value<short>()->required(), "network port to communicate on with the remote host")
      ;

      if (vm.count("help")) {
        std::cerr << clientDesc << std::endl;
        exit(1);
      }

      std::vector<std::string> clientOpts = po::collect_unrecognized(parsed.options, po::include_positional);
      clientOpts.erase(clientOpts.begin());
      po::store(po::command_line_parser(clientOpts).options(clientDesc).positional(clientPos).run(), vm);
      po::notify(vm);
      
      opts.mode = p2p::Session::Mode::Client;
      opts.host = vm.count("server") ? vm["server"].as<std::string>() : "";
      opts.port = vm.count("port") ? vm["port"].as<short>() : 0;
    }
    else
    {
      std::cerr << desc << std::endl;
      exit(1);
    }
  }
  catch (std::exception &ex)
  {
    throw;
  }
}

int main(int argc, char** argv)
{
  Options options;
  try {
    HandleCommandLine(options, argc, argv);
  } catch (std::exception &ex) {
    std::cerr << "Command line error: " << ex.what() << std::endl;
    return 1;
  }

  boost::asio::io_service ioService;
  boost::asio::io_service::work work(ioService);

  std::thread serviceRunner([&ioService]()
    {
      try {
        ioService.run();
      } catch (...) {
        std::cout << "Exception thrown in service runner thread." << std::endl;
      }
    });

  try
  {
    std::cout << "Starting game..." << std::endl;
    p2p::Session session(ioService, options.mode, options.host, options.port);
    session.run ();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
  }

  // Really not a nice solution. Copied from asio example "async daytime client"
  ioService.stop();
  serviceRunner.join();

  std::cout << "Game ended." << std::endl;
  return 0;
}

