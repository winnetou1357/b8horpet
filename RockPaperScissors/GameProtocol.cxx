#include <string>
#include <utility>
#include <iostream>

#include "GameLogic.hxx"

#include "GameProtocol.hxx"

rps::GameProtocol::GameProtocol(rps::GameLogic& logic_in, rps::io::SymmetricDevice& device_in, BuilderPtr&& builder_in, ParserPtr&& parser_in)
    : logic(logic_in)
    , device(device_in)
    , builder(std::move(builder_in))
    , parser(std::move(parser_in))
{
    // TODO how to initiate game? what about player ID?
}

bool rps::GameProtocol::doRound() const
{
    try
    {
        // TODO it should random-generate the salt and return it for storing in the session
        std::string salt = "salt";
        rps::Choice choice = logic.GetNextChoice();
        std::cout << "My choice: " << char(choice) << std::endl;

        rps::io::message_t opponentEncryptedChoice = device.asyncSendAndReceive(builder->buildChoiceMessage(choice, salt));

//        // TODO debug code
//        std::cout << "Encrypted choice: ";
//        std::for_each(opponentEncryptedChoice.cbegin(), opponentEncryptedChoice.cend(), [&](const char &c) {
//          printf("%02hhx", c);
//        });
//        std::cout << std::endl;

        std::string opponentSalt = parser->parseSaltMessage(device.asyncSendAndReceive(builder->buildSaltMessage(salt)));
//         std::cout << "Opponent salt (" << opponentSalt.size() << "): " << opponentSalt << std::endl;
        rps::Choice opponentChoice = parser->parseChoiceMessage(opponentEncryptedChoice, opponentSalt);
//         std::cout << "Opponent choice: " << char(opponentChoice) << std::endl;

        rps::RoundResult result = logic.EvaluateRound(opponentChoice);

        std::cout << "Round result: ";
        switch(result)
        {
          case rps::RoundResult::Tie:
            std::cout << "TIE";
            break;
          case rps::RoundResult::Win:
            std::cout << "WON";
            break;
          case rps::RoundResult::Lose:
            std::cout << "LOSE";
            break;
          default:
            std::cout << "WTF???";
            break;
        }
        std::cout << std::endl;

        // TODO log round result elsewhere?

        return true;  // resume game
    }
    catch (const std::exception& ex)
    {
        // TODO log exception
        std::cerr << "Error while playing: " << ex.what() << std::endl;
        return false;
    }
}
