#include <future>

#include <boost/asio/use_future.hpp>

#include "SymmetricDevice.hxx"

rps::io::message_t rps::io::SymmetricDevice::asyncSendAndReceive(const rps::io::message_t& message)
{
  std::future<std::size_t> write = boost::asio::async_write(mSocket, boost::asio::buffer(message, message.size()), boost::asio::use_future);

  rps::io::message_t received;
  char inData[1024];
  std::fill(inData, inData + sizeof(inData), '\0');
  std::future<std::size_t> read = mSocket.async_read_some(boost::asio::buffer(inData, sizeof(inData)), boost::asio::use_future);

  std::size_t writtenBytes  = write.get();
  std::size_t readBytes     = read.get();

  // TODO error checking: try-catch + byte counts
  return message_t(inData, inData + strlen(inData));
}
