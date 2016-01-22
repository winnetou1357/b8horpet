#ifndef RPS_IO_SYMMETRICDEVICE_HXX
#define RPS_IO_SYMMETRICDEVICE_HXX

#include <boost/asio.hpp>

#include "MD5.hxx"
#include "Message.hxx"

namespace rps {
namespace io {

  /**
   * Implementation of the communication protocol between players (peers).
   */
  class SymmetricDevice
  {
  private:
    boost::asio::ip::tcp::socket& mSocket;
  public:
    class CommunicationError : public std::exception {
      public:
        const char* what () const throw () override {
          return "Communication error";
        }
    };

    SymmetricDevice (boost::asio::ip::tcp::socket& socket) : mSocket(socket) { }

    rps::io::message_t asyncSendAndReceive(const rps::io::message_t& message);
  };

} // namespace io
} // namespace rps

  #endif // RPS_IO_SYMMETRICDEVICE_HXX
