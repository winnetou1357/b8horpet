#include <set>
#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "Options.hxx"
#include "CommandLine.hxx"

void rps::commandline::Handle(rps::Options &opts, int argc, char **argv)
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

      opts.mode = Options::Mode::Server;
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
      
      opts.mode = rps::Options::Mode::Client;
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
