#ifndef SRC_MAPPERS_RESOURCE_RESOURCEMAPPER_H
#define SRC_MAPPERS_RESOURCE_RESOURCEMAPPER_H

#include <map>
#include <set>
#include <string>

#include "src/vic3_world/buildings/building.h"



namespace mappers
{

struct ResourceScore
{
   std::map<std::string, float> base;
   std::set<std::string> bonus_buildings;
   float bonus;
   float total;
};

class ResourceMapper
{
  public:
   explicit ResourceMapper(std::map<std::string, ResourceScore> scores): scores_(std::move(scores)) {}

   [[nodiscard]] float CalculateScore(const std::string& hoi_resource,
       const std::vector<vic3::Building>& buildings) const;
   [[nodiscard]] float WorldTotal(const std::string& hoi_resource) const;

  private:
   std::map<std::string, ResourceScore> scores_;
};

}  // namespace mappers


#endif  // SRC_MAPPERS_RESOURCE_RESOURCEMAPPER_H
