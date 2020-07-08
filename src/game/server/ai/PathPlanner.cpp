#include "PathPlanner.h"

#include <base/RayTracing.h>
#include <base/utils.h>
#include <game/server/Trigger.h>
#include <game/server/EntityEnumerations.h>
#include "NavGraphEdge.h"
#include <game/server/Map.h>
#include <base/debug.h>


PathPlanner::PathPlanner() :
   m_pMap(0),
   m_pShortestPaths(0),
   m_pShortestCostsPaths(0),
   m_powerupTrigMaps(NUM_POWERUPS, 0),
   m_weaponTrigMaps(NUM_WEAPONS, 0)
{}

PathPlanner::PathPlanner(const PathPlanner&) {}

PathPlanner::~PathPlanner()
{}

PathPlanner& PathPlanner::operator=(const PathPlanner&) {}

PathPlanner& PathPlanner::getInstance()
{
   static PathPlanner pp;
   return pp;
}


std::list<NavGraphEdge*> PathPlanner::getShortestPath(int from, int to)const
{
   std::list<NavGraphEdge*> shortestPath;
   int fromCurNode = from;
   int toCurNode = -1; 
   
   while ( toCurNode != to )
   {
      toCurNode = (*m_pShortestPaths)[fromCurNode][to];

      NavGraphEdge* pEdge = m_pMap->GetNavGraph().GetEdge(fromCurNode, toCurNode);
      pEdge->setOnPath(true);

      // opposite edge must be colored if is float
      if (m_pMap->GetNavGraph().isEdgePresent(toCurNode, fromCurNode))
      {
         NavGraphEdge* pOppositeEdge = m_pMap->GetNavGraph().GetEdge(toCurNode, fromCurNode);
         pOppositeEdge->setOnPath(true);
      }

      shortestPath.push_back( pEdge );

      fromCurNode = toCurNode;
   }

   return shortestPath;
}


float PathPlanner::getLowestPathCost(int from, int to)const
{
   return (*m_pShortestCostsPaths)[from][to];
}

int PathPlanner::getClosestNodeToItem(int from, int itemType, int type) const
{
   int minCostIdx = -1;
   float minCost = MaxInt;

    // Choose the right triggerMap
   switch(itemType)
   {
      case IT_POWERUP:
         for(TriggerMap::const_iterator it = m_powerupTrigMaps[type]->begin();
         it != m_powerupTrigMaps[type]->end();
         ++it)
         {
            Trigger* trig = (*it).second;

            // trigger must be active to be available
            if( trig->isActive() )
            {
               float curCost = getLowestPathCost(from, (*it).first);

               if( curCost < minCost )
               {
                  minCost = curCost;
                  minCostIdx = (*it).first;
               }
            }
         }
         break;

      case IT_WEAPON:
         for(TriggerMap::const_iterator it = m_weaponTrigMaps[type]->begin();
         it != m_weaponTrigMaps[type]->end();
         ++it)
      {
         Trigger* trig = (*it).second;

         // trigger must be active to be available
         if( trig->isActive() )
         {
            float curCost = getLowestPathCost(from, (*it).first);

            if( curCost < minCost )
            {
               minCost = curCost;
               minCostIdx = (*it).first;
            }
         }
      }
      break;

      default:
         assert ( 0 && "Item type invalid");
         break;

   }

   return minCostIdx;
}


float PathPlanner::getCostToClosestItem(const Vector2D& pos, int itemType, int type) const
{
   int from = getClosestNode(pos);
   int to = getClosestNodeToItem(from, itemType, type);

   if (to == -1) {return -1.0f;}

   return (*m_pShortestCostsPaths)[from][to];
}


int PathPlanner::getClosestNode(const Vector2D& pos)const
{
   const CellSpacePartition<Waypoint*>& cs = m_pMap->GetCellSpace();

   int  curCellIdx = cs.PositionToIndex(pos);
   const Cell<Waypoint*>& curCell = cs.getCellByIndex(curCellIdx);

   float minDist = MaxInt;
   const std::list<Waypoint*>& members = curCell.getMembers();

   std::list<Waypoint*>::const_iterator curMember = members.begin();
   const Waypoint* wptChosen = *curMember;

   for (curMember = members.begin(); curMember!= members.end(); ++curMember)
   {
      Waypoint* curWpt = *curMember;
      float curDist = pos.Distance(curWpt->Position());

      Vector2D targPos( curWpt->Position() );
      if ( curDist < minDist && RayTracing::canTraceRay(pos, targPos, m_pMap) )
      {
         minDist = curDist;
         wptChosen = curWpt;
      }
   }

   return wptChosen->Index();
}


int PathPlanner::getRandomNode(int avoidedIdx)const
{
   int randIdx = avoidedIdx;
   while( (randIdx = RandInt(0, m_pMap->GetNavGraph().NumNodes()-1)) == avoidedIdx );
   return randIdx;
}

std::list<NavGraphEdge*> PathPlanner::getPathToItem( int from, int itemType, int type) const
{
   int to = getClosestNodeToItem(from, itemType, type);
   return getShortestPath(from, to);
}

void PathPlanner::setShortestPaths(std::vector< std::vector<int> > v)
{
   delete m_pShortestPaths;
   m_pShortestPaths = 0;

   m_pShortestPaths = new std::vector< std::vector<int> > (v);
}

void PathPlanner::setShortestCostsPaths(std::vector< std::vector<float> > v)
{
   delete m_pShortestCostsPaths;
   m_pShortestCostsPaths = 0;

   m_pShortestCostsPaths = new std::vector< std::vector<float> > (v);
}

void PathPlanner::setMap(const Map* pMap)
{
   m_pMap = pMap;
}

void PathPlanner::createTriggerMaps()
{
   m_powerupTrigMaps.resize(NUM_POWERUPS, 0);
   m_weaponTrigMaps.resize(NUM_WEAPONS, 0);
}

void PathPlanner::setPowerupTrigMap(int type, TriggerMap* pTriggerMap)
{
   assert (m_powerupTrigMaps.size() > type);
   m_powerupTrigMaps.at(type) = pTriggerMap;
}

void PathPlanner::setWeaponTrigMap(int type, TriggerMap* pTriggerMap)
{
   m_weaponTrigMaps.at(type) = pTriggerMap;
}

Trigger* PathPlanner::getTrigger(int itemType, int type, int nodeIdx)
{

   switch(itemType)
   {
      case IT_POWERUP:
         return (*m_powerupTrigMaps.at(type))[nodeIdx];

      case IT_WEAPON:
         return (*m_weaponTrigMaps.at(type))[nodeIdx];

      default:
         assert ( 0 && "Item type invalid");
         break;
   }

}
