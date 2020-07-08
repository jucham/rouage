#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include <list>
#include <map>
#include <vector>

class Map;
class Trigger;
class NavGraphEdge;
class Vector2D;

///
/// \class PathPlanner PathPlanner.h
/// \brief This class lets bot to find and planify his moves through the map
///
///  class contains precalculed tables, one to get shortest path from one
/// node to an other node, one to get shortest path cost from one
/// node to an other node. For that bot is able to grab item, class contains
/// trigger maps. they are an association of a graph node index (key) and a pointer
/// to a trigger (value).
///
class PathPlanner
{
   public:

      typedef std::map<int, Trigger*> TriggerMap;

      /// \brief returns a unique path planner instance
      static PathPlanner& getInstance();

      /// \brief Returns a list of edges represents shortest path between two nodes
      std::list<NavGraphEdge*> getShortestPath(int from, int to)const;

      /// \brief Returns a float value represents lowest path cost between two nodes
      float getLowestPathCost(int from, int to) const;

      /// \brief Returns an node index that is the closest node to reach to grab
      /// a given item
      int getClosestNodeToItem(int from, int itemType, int type) const;

      /// \brief for a given item find the closest and returns cost to reach its
      float getCostToClosestItem(const Vector2D& pos, int itemType, int type) const;

      /// \brief returns the closest node of the position \a pos
      int getClosestNode(const Vector2D& pos)const;

      /// \brief returns a random node index that mustn't be \a avoidedIdx
      int getRandomNode(int avoidedIdx)const;

      /// \brief returns an edge list that is a path to items
      std::list<NavGraphEdge*> getPathToItem( int from, int itemType, int type) const;

      void setShortestPaths(std::vector< std::vector<int> > v);
      void setShortestCostsPaths(std::vector< std::vector<float> > v);

      /// \brief set current map
      void setMap(const Map* pMap);

      void createTriggerMaps();

      /// \brief set a trigger map for a given powerup
      void setPowerupTrigMap(int type, TriggerMap* pTriggerMap);

      /// \brief set a trigger map for a given weapon
      void setWeaponTrigMap(int type, TriggerMap* pTriggerMap);

      /// \brief returns pointer to a trigger spotted on a node
      ///
      /// \a itemType and \a type let choose the right trigger map and then
      /// choose the right trigger with node index
      Trigger* getTrigger(int itemType, int type, int nodeIdx);

      //TODO: write a function to reset path planner when loading a nother map
      // void reset()

   private:

      const Map*                           m_pMap;
      std::vector< std::vector<int> >*     m_pShortestPaths;      ///< 2D array contains shortest paths between all nodes
      std::vector< std::vector<float> >*   m_pShortestCostsPaths; ///< 2D array contains shortest path cost between all nodes
      std::vector<TriggerMap*>             m_powerupTrigMaps;     ///< trigger map of powerups
      std::vector<TriggerMap*>             m_weaponTrigMaps;      ///< trigger map of weapons

      PathPlanner();
      PathPlanner(const PathPlanner&);
      ~PathPlanner();

      PathPlanner& operator=(const PathPlanner&);

};

#endif // PATHPLANNER_H
