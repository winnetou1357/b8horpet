#include <memory>
#include <string>
#include <iostream>
#include <exception>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "GameLogic.hxx"
#include "SymmetricDevice.hxx"
#include "GameProtocol.hxx"

#include "Session.hxx"

rps::Session::Session(boost::asio::io_service& ioService, rps::Options::Mode mode, const std::string& host, short int port)
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

rps::Session::~Session()
{
  if (mSocket.is_open()) {
      mSocket.close ();
  }
}

void rps::Session::run()
{
  try {
    boost::asio::ip::tcp::resolver::iterator iter
    = mResolver.async_resolve(boost::asio::ip::tcp::resolver::query(boost::asio::ip::tcp::v4(), mHost, std::to_string(mPort)), boost::asio::use_future).get();
    if (iter == boost::asio::ip::tcp::resolver::iterator()) {
      std::cout << "Could not resolve hostname " << mHost << std::endl;
    }
    else {
      std::for_each(iter, boost::asio::ip::tcp::resolver::iterator(), [&](const boost::asio::ip::tcp::endpoint& endpoint) {
          std::cout << "Resolved endpoint " << endpoint << std::endl;
      });

      connect(iter);
      play();
    }
  }
  catch (std::system_error& ex) {
      std::cerr << "Error while resolving hostname " << ex.what() << std::endl;
  }
}

void rps::Session::connect(const boost::asio::ip::tcp::resolver::iterator& endpointIterator)
{
  try {
    if (mMode == rps::Options::Mode::Client) {
      boost::asio::async_connect(mSocket, endpointIterator, boost::asio::use_future).get();
    } else if (mMode == rps::Options::Mode::Server) {
      boost::asio::ip::tcp::acceptor acceptor(mIoService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), mPort));

      std::cout << "Waiting for connection on port " << mPort << std::endl;
      acceptor.async_accept(mSocket, boost::asio::use_future).get();
    } else {
        throw InvalidMode();
    }
  }
  catch (const std::system_error& ex) {
      std::cerr << "Error while connecting " << ex.what() << std::endl;
      throw;
  }
}

void rps::Session::play()
{
  try {
      static const std::string playerId = "1234";

      rps::GameLogic logic;
      rps::io::SymmetricDevice sd(mSocket);
      rps::GameProtocol protocol(logic, sd, std::unique_ptr<rps::io::MessageBuilder>(new rps::io::MessageBuilder()), std::unique_ptr<rps::io::MessageParser>(new rps::io::MessageParser()));
      while (protocol.doRound()) {
          std::cout << "Next round" << std::endl;
      }
  } catch (const std::exception& ex) {
      std::cerr << "Error while playing game: " << ex.what() << std::endl;
      throw;
  }
}
