#ifndef SRC_HOI4WORLD_MAP_RAILWAYS_H
#define SRC_HOI4WORLD_MAP_RAILWAYS_H



#include <set>
#include <vector>

#include "src/hoi4_world/map/railway.h"



namespace hoi4
{

struct Railways
{
   std::vector<Railway> railways;
   std::set<int> supply_nodes;
};

}  // namespace hoi4



#endif  // SRC_HOI4WORLD_MAP_RAILWAYS_H