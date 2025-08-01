#ifndef SRC_VIC3WORLD_PACTS_PACT_H
#define SRC_VIC3WORLD_PACTS_PACT_H



#include <external/commonItems/Date.h>

#include <istream>
#include <optional>
#include <set>



namespace vic3
{
// This could be expanded into a mapper between Vic3 pact types and HoI4 autonomous_states
static const std::set<std::string> kSubjectPactTypes =
    {"protectorate", "puppet", "dominion", "personal_union", "vassal", "tributary", "chartered_company", "colony"};

class Pact
{
  public:
   Pact(int first_id, int second_id, std::string action, date start_date, std::optional<int> forced_duration):
       first_id_(first_id),
       second_id_(second_id),
       action_(std::move(action)),
       start_date_(std::move(start_date)),
       forced_duration_(std::move(forced_duration))
   {
   }

   [[nodiscard]] int GetFirstId() const { return first_id_; }
   [[nodiscard]] int GetSecondId() const { return second_id_; }
   [[nodiscard]] std::string GetAction() const { return action_; }
   [[nodiscard]] date GetStartDate() const { return start_date_; }
   [[nodiscard]] std::optional<int> GetForcedDuration() const { return forced_duration_; }
   [[nodiscard]] bool IsSubjectRelationship() const
   {
      return kSubjectPactTypes.find(action_) != kSubjectPactTypes.end();
   }

   [[nodiscard]] std::partial_ordering operator<=>(const Pact&) const = default;

  private:
   // In vassal-type relationships, first_id_ is the id of the overlord, second_id_ is the id of the vassal.
   int first_id_ = 0;
   int second_id_ = 0;
   std::string action_;
   date start_date_;
   std::optional<int> forced_duration_;
};
}  // namespace vic3



#endif  // SRC_VIC3WORLD_PACTS_PACT_H