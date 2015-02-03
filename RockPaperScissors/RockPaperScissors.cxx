#include <string>
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/system/error_code.hpp>

#include "RockPaperScissorsConfig.hxx"

#include "Options.hxx"
#include "CommandLine.hxx"

namespace rps {
  
  using namespace boost::asio;

  /**
   * Abstract session base class.
   */
  class Session
  {
  public:
    class InvalidMode : public std::exception
    {
    public:
      const char* what() const throw() override {
        return "Invalid session mode";
      }
    };
  
    Session (io_service& ioService, rps::Options::Mode mode, const std::string& host, short port)
      : mIoService(ioService)
      , mResolver(ioService)
      , mSocket(ioService)
      , mMode(mode)
      , mHost(host)
      , mPort(port)
    {
      switch (mMode)
      {
        case rps::Options::Mode::Server:
          mHost = "";
          break;
        case rps::Options::Mode::Client:
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
        if (mMode == rps::Options::Mode::Client) {
          boost::asio::async_connect(mSocket, endpointIterator, boost::asio::use_future).get();
        } else if (mMode == rps::Options::Mode::Server) {
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
	
    rps::Options::Mode  mMode;
    std::string         mHost;
    short               mPort;
    
  };
  
} // namespace rps

int main(int argc, char** argv)
{
  namespace cl = rps::commandline;
  
  rps::Options options;
  try {
    cl::Handle(options, argc, argv);
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
    rps::Session session(ioService, options.mode, options.host, options.port);
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

