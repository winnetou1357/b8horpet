#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "MD5.hxx"

#include "Message.hxx"

rps::io::message_t rps::io::MessageBuilder::buildChoiceMessage(rps::Choice c, const std::string& salt)
{
    using namespace rps::crypto;
    std::stringstream ss;
    ss << salt << static_cast<char>(c) << salt;
    rps::crypto::md5::digest digest
      = rps::crypto::md5::generate(reinterpret_cast<const md5::byte*>(ss.str().c_str()), ss.str().size());
    rps::io::message_t encryptedChoiceMessage;
    std::copy(digest.cbegin(), digest.cend(), std::back_inserter(encryptedChoiceMessage));

     // TODO debug code
     std::cout << "My encrypted choice: " << encryptedChoiceMessage << std::endl;

    return encryptedChoiceMessage;
}

rps::io::message_t rps::io::MessageBuilder::buildSaltMessage(const std::string& salt)
{
  rps::io::message_t message;
  std::copy(salt.cbegin(), salt.cend(), std::back_inserter(message));
  return message;
}

rps::Choice rps::io::MessageParser::parseChoiceMessage(const rps::io::message_t& message, const std::string& salt)
{
  rps::Choice allChoices[] = { rps::Choice::Rock, rps::Choice::Paper, rps::Choice::Scissors };
  for (const rps::Choice& c : allChoices)
  {
    if (rps::io::MessageBuilder().buildChoiceMessage(c, salt) == message)
    {
      return c;
    }
  }

    throw ParserError();
}

std::string rps::io::MessageParser::parseSaltMessage(const io::message_t& message)
{
  std::string salt;
  std::copy(message.cbegin(), message.cend(), std::back_inserter(salt));
  return salt;
}

std::ostream& operator<< (std::ostream& os, const rps::io::message_t& message)
{
  std::for_each(message.cbegin(), message.cend(), [&](const char &c) {
    os << std::setw(2) << std::setfill('0') << std::hex << c;
  });

  return os;
}
