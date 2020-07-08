#include "GoalEvaluator.h"

GoalEvaluator::GoalEvaluator(float CharacterBias, std::string name) :
         m_fCharacterBias(CharacterBias),
         m_fLastDesirability(0.0f),
         m_sName(name)
         {}

GoalEvaluator::~GoalEvaluator() {}
