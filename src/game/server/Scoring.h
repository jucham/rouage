#ifndef SCORING_H
#define SCORING_H

#include <vector>
#include <string>
#include <algorithm>

class Character;

struct CharScore
{
   int kill;
   int death;
};

struct CompScore
{
    bool operator ()( const CharScore & cs1, const CharScore & cs2 ) const
    {
        return cs1.kill > cs2.kill;
    }
};


class Scoring
{
   public:
      Scoring(const std::vector<Character*>& chars);

      void addKill(const std::string& name);
      void addDeath(const std::string& name);

      const std::vector<CharScore>& Scores();

   protected:
      std::vector<CharScore> m_Scores;
};

#endif // SCORING_H
