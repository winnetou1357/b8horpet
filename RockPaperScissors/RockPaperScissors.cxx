#include <string>
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/system/error_code.hpp>

#include "RockPaperScissorsConfig.hxx"

#include "Options.hxx"
#include "CommandLine.hxx"
#include "Session.hxx"

int main(int argc, char** argv)
{
  rps::Options options;
  try {
    rps::commandline::Handle(options, argc, argv);
  } catch (std::exception &ex) {
    std::cerr << "Command line error: " << ex.what() << std::endl;
    return 1;
  }

  boost::asio::io_service ioService;
  boost::asio::io_service::work work(ioService);

  std::thread serviceRunner([&ioService]() {
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


