#include "Scoring.h"

#include "Character.h"

Scoring::Scoring(const std::vector<Character*>& chars)
{
	m_Scores.resize(chars.size());

	for (int clientId = 0; clientId < MAX_CLIENTS; clientId++)
	{
		m_Scores[clientId].kill = 0;
		m_Scores[clientId].death = 0;
	}
}

void Scoring::addKill(const std::string& name)
{
	for (int i = 0; i < (int) m_Scores.size(); i++)
	{

	}
}

void Scoring::addDeath(const std::string& name)
{
	for (int i = 0; i < (int) m_Scores.size(); i++)
	{

	}
}
