#include "src/hoi4_world/roles/stories_creator.h"

#include <external/fmt/include/fmt/format.h>

#include <ranges>
#include <set>

#include "src/hoi4_world/roles/roles_importer.h"



namespace
{

using Tag = std::string;
using CombinationName = std::string;


bool IsRoleValidForCountry(const hoi4::Role& role, const hoi4::World& world, const hoi4::Country& country)
{
   return role.GetTrigger().IsValid(hoi4::CountryScope{country}, world);
}


std::optional<std::vector<std::pair<Tag, CombinationName>>> MakeCombinations(
    const std::map<std::string, hoi4::Role>& roles,
    const hoi4::World& world,
    const std::map<std::string, hoi4::Country>& countries)
{
   std::vector<std::pair<Tag, CombinationName>> combinations;

   for (const auto& [role_name, role]: roles)
   {
      for (const auto& [country_tag, country]: countries)
      {
         if (IsRoleValidForCountry(role, world, country))
         {
            combinations.emplace_back(country_tag, role_name);
         }
      }
   }

   Log(LogLevel::Info) << fmt::format("\tCreated {} role combinations.", combinations.size());
   return combinations;
}


std::optional<std::vector<std::pair<Tag, CombinationName>>> SortCombinations(
    std::vector<std::pair<Tag, CombinationName>> combinations,
    const std::map<std::string, hoi4::Role>& roles)
{
   std::ranges::sort(combinations, [roles](const auto& a, const auto& b) {
      float a_score = 0.0F;
      if (const auto role = roles.find(a.second); role != roles.end())
      {
         a_score = role->second.GetScore();
      }
      float b_score = 0.0F;
      if (const auto role = roles.find(b.second); role != roles.end())
      {
         b_score = role->second.GetScore();
      }

      if (a_score != b_score)
      {
         // higher scores go first
         return a_score > b_score;
      }
      if (a.first != b.first)
      {
         // lexicographically lower tags go first
         return a.first < b.first;
      }

      // lexicographically lower role names go first
      return a.second < b.second;
   });

   return combinations;
}


std::optional<std::vector<std::pair<Tag, CombinationName>>> FilterCombinations(
    std::vector<std::pair<Tag, CombinationName>> combinations,
    const std::map<std::string, hoi4::Role>& roles)
{
   std::set<std::pair<Tag, std::string>> blocked_combinations;

   const auto [first, last] = std::ranges::remove_if(combinations,
       [&blocked_combinations, roles](const std::pair<Tag, CombinationName>& combination) {
          const auto& role = roles.find(combination.second);
          if (role == roles.end())
          {
             return true;
          }

          if (blocked_combinations.contains(combination))
          {
             return true;
          }
          if (blocked_combinations.contains(std::make_pair(combination.first, role->second.GetCategory())))
          {
             return true;
          }

          for (const std::string blocker: role->second.GetBlockers())
          {
             blocked_combinations.emplace(std::make_pair(combination.first, blocker));
          }

          return false;
       });
   combinations.erase(first, last);

   Log(LogLevel::Info) << fmt::format("\tFiltered to {} role combinations.", combinations.size());
   return combinations;
}


std::optional<std::vector<std::pair<Tag, hoi4::Role>>> ExpandCombinations(
    const std::vector<std::pair<Tag, CombinationName>>& combinations,
    const std::map<std::string, hoi4::Role>& roles)
{
   std::vector<std::pair<Tag, hoi4::Role>> expanded_combinations;

   for (const auto& [tag, role_name]: combinations)
   {
      const auto& role = roles.find(role_name);
      if (role == roles.end())
      {
         return std::nullopt;
      }

      expanded_combinations.emplace_back(tag, role->second);
   }

   return expanded_combinations;
}


std::optional<std::map<Tag, std::vector<hoi4::Role>>> GroupCombinations(
    const std::vector<std::pair<Tag, hoi4::Role>>& combinations)
{
   std::map<Tag, std::vector<hoi4::Role>> grouped_combinations;
   for (const auto& [tag, role]: combinations)
   {
      auto [itr, success] = grouped_combinations.emplace(tag, std::vector{role});
      if (!success)
      {
         itr->second.push_back(role);
      }
   }

   return grouped_combinations;
}

}  // namespace



std::map<Tag, std::vector<hoi4::Role>> hoi4::CreateStories(const std::map<std::string, hoi4::Role>& roles,
    const hoi4::World& world,
    const std::map<std::string, hoi4::Country>& countries)
{
   Log(LogLevel::Info) << "Writing stories";
   return MakeCombinations(roles, world, countries)
       .and_then([roles](std::vector<std::pair<Tag, CombinationName>> combinations) {
          return SortCombinations(combinations, roles);
       })
       .and_then([roles](std::vector<std::pair<Tag, CombinationName>> combinations) {
          return FilterCombinations(combinations, roles);
       })
       .and_then([roles](std::vector<std::pair<Tag, CombinationName>> combinations) {
          return ExpandCombinations(combinations, roles);
       })
       .and_then([](std::vector<std::pair<Tag, hoi4::Role>> combinations) {
          return GroupCombinations(combinations);
       })
       .value_or(std::map<Tag, std::vector<hoi4::Role>>{});
}