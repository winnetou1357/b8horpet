#ifndef RPS_GAMEPROTOCOL_HXX
#define RPS_GAMEPROTOCOL_HXX

#include <memory>

#include "SymmetricDevice.hxx"
#include "Message.hxx"

namespace rps {

  class GameProtocol
  {
  public:
    typedef std::unique_ptr<rps::io::MessageBuilder> BuilderPtr;
    typedef std::unique_ptr<rps::io::MessageParser> ParserPtr;

    GameProtocol(rps::GameLogic& logic_in, rps::io::SymmetricDevice& device_in, BuilderPtr&& builder_in, ParserPtr&& parser_in);

    bool doRound() const;

  private:
    rps::GameLogic&           logic;
    rps::io::SymmetricDevice& device;
    BuilderPtr                builder;
    ParserPtr                 parser;
  };

} // namespace rps

#endif // RPS_GAMEPROTOCOL_HXX
