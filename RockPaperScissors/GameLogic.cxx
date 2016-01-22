#include "GameLogic.hxx"

/**
  * rps::GameLogic methods
  */

rps::Choice rps::GameLogic::GetNextChoice()
{
  // TODO random strategy
  return lastChoice = Choice::Rock;
}

rps::RoundResult rps::GameLogic::EvaluateRound(rps::Choice oc)
{
  if (lastChoice == oc) {
    return RoundResult::Tie;
  }

  switch (lastChoice) {
    case Choice::Rock:
      return (oc == Choice::Paper) ? RoundResult::Lose : RoundResult::Win;
      break;
    case Choice::Paper:
      return (oc == Choice::Scissors) ? RoundResult::Lose : RoundResult::Win;
      break;
    case Choice::Scissors:
      return (oc == Choice::Rock) ? RoundResult::Lose : RoundResult::Win;
      break;
    default:
      // TODO assert unknown value of choice
      break;
  }
}
