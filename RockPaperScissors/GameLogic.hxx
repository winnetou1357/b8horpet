#ifndef RPS_GAMELOGIC_HXX
#define RPS_GAMELOGIC_HXX

#include "GameLogicFwd.hxx"

namespace rps {

  enum class Choice : char
  {
    Rock      = 'r',
    Paper     = 'p',
    Scissors  = 's',
  };

  enum class RoundResult
  {
    Lose,
    Win,
    Tie,
  };

  /**
    * Stands for managing a round in the game, namely:
    * which option to choose in that round,
    * what is the result of the round.
    */
  class GameLogic
  {
  public:
    Choice GetNextChoice();
    RoundResult EvaluateRound(Choice oc);

  private:
    Choice lastChoice;
  };

} // namespace rps

#endif // RPS_GAMELOGIC_HXX
