#include <string>
#include <iostream>

#include <boost/asio.hpp>
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
    Session (io_service& ioService, const std::string& host, short port)
      : mIoService (ioService)
      , mResolver (ioService)
      , mSocket (ioService)
      , mHost (host)
      , mPort (port)
    {
      // do nothing
    }

    ~Session ()
    {
      if (mSocket.is_open())
      {
        mSocket.close ();
      }
    }

    void run ()
    {
      mResolver.async_resolve (ip::tcp::resolver::query (ip::tcp::v4 (), mHost, std::to_string (mPort)), [&](const boost::system::error_code& error, const ip::tcp::resolver::iterator& iter)
      {
        std::cout << "resolve with error: " << error << std::endl;
        if (!error && iter != ip::tcp::resolver::iterator ())
        {
          connectAndListen (*iter);
        }
      });
    }

    void connectAndListen (const ip::tcp::endpoint& endpoint)
    {
      std::cout << "Connect and listen to " << endpoint << std::endl;
    }

  private:
    io_service& mIoService;
    ip::tcp::resolver mResolver;
    ip::tcp::socket mSocket;

    const std::string& mHost;
    short mPort;
  };
}


int main (int argc, char** argv)
{
  try
  {
    if (argc != 3)
    {
      std::cout << "Usage: RockPaperScissors HOST PORT" << std::endl;
      return 1;
    }

    boost::asio::io_service ioService;
    p2p::Session session (ioService, argv[1], std::stoi (argv[2]));
    session.run ();
    std::cout << "Starting game..." << std::endl;
    ioService.run ();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  std::cout << "Game ended." << std::endl;
  return 0;
}

