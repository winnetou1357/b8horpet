#ifndef RPS_IO_MESSAGE_HXX
#define RPS_IO_MESSAGE_HXX

#include <algorithm>

#include "GameLogic.hxx"

namespace rps {
namespace io {

  typedef std::vector<char> message_t;

  class MessageBuilder
  {
  public:
    message_t buildChoiceMessage(rps::Choice c, const std::string& salt);
    message_t buildSaltMessage(const std::string& salt);
  };

  class MessageParser
  {
  public:
    class ParserError : public std::exception
    {
    public:
      const char* what () const throw () override
      {
        return "Failed to parse a message";
      }
    };

    rps::Choice parseChoiceMessage(const message_t& message, const std::string& salt);
    std::string parseSaltMessage(const message_t& message);
  };

} // namespace io
} // namespace rps

std::ostream& operator<< (std::ostream& os, const rps::io::message_t& message);

#endif // RPS_IO_MESSAGE_HXX
