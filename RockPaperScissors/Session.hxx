#ifndef RPS_SESSION_HXX
#define RPS_SESSION_HXX

#include <exception>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "Options.hxx"

namespace rps {

  /**
   * Session class manages a game session: a series of rounds until game end,
   * along with control, IO and logging.
   */
  class Session
  {
  public:
    class InvalidMode : public std::exception
    {
    public:
      const char* what () const throw () override {
        return "Invalid session mode";
      }
    };

    Session(boost::asio::io_service& ioService, Options::Mode mode, const std::string& host, short port);
    ~Session();

    void run();

  protected:
    void connect(const boost::asio::ip::tcp::resolver::iterator& endpointIterator);
    void play();

  private:
    boost::asio::io_service&        mIoService;

    boost::asio::ip::tcp::resolver  mResolver;
    boost::asio::ip::tcp::socket    mSocket;

    rps::Options::Mode              mMode;
    std::string                     mHost;
    short                           mPort;
  };

} // namespace rps

#endif // RPS_SESSION_HXX
