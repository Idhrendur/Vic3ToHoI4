#include "src/vic3_world/database/database_parser.h"

#include <external/commonItems/CommonRegexes.h>
#include <external/commonItems/ParserHelpers.h>



namespace vic3
{

DatabaseParser::DatabaseParser(): commonItems::parser(), db_entry_parser_()
{
   this->db_entry_parser_.registerRegex(commonItems::integerRegex,
       []([[maybe_unused]] const std::string& key, [[maybe_unused]] std::istream& stream) {
          return;
       });
   this->db_entry_parser_.IgnoreUnregisteredItems();
   this->registerKeyword("database", db_entry_parser_);
   this->IgnoreAndLogUnregisteredItems();
}

void SkipSpaces(std::istream& stream)
{
   while (stream.peek() == ' ')
   {
      stream.get();
   }
}

DatabaseParser::DatabaseParser(commonItems::parsingFunctionStreamOnly single_element_func):
    commonItems::parser(),
    db_entry_parser_()
{
   this->db_entry_parser_.registerRegex(commonItems::integerRegex,
       [single_element_func](std::string key, std::istream& stream) {
          // i have a mouth and i must scream
          SkipSpaces(stream);
          const auto m = static_cast<char>(stream.get());
          const auto n = static_cast<char>(stream.peek());
          if (n == ' ')
          {
             stream.get();
          }
          if (stream.peek() == '{')
          {
             if (n == ' ')
             {
                stream.putback(n);
             }
             stream.putback(m);
             single_element_func(stream);
          }
          else
          {
             commonItems::ignoreItem(key, stream);
          }
       });
   this->db_entry_parser_.IgnoreAndLogUnregisteredItems();

   this->registerKeyword("database", this->db_entry_parser_);
   this->IgnoreAndLogUnregisteredItems();
}

DatabaseParser::DatabaseParser(commonItems::parsingFunction multi_element_func):
    commonItems::parser(),
    db_entry_parser_()
{
   this->db_entry_parser_.registerRegex(commonItems::integerRegex,
       [multi_element_func](std::string key, std::istream& stream) {
          // i have a mouth and i must scream
          SkipSpaces(stream);
          const auto m = static_cast<char>(stream.get());
          const auto n = static_cast<char>(stream.peek());
          if (n == ' ')
          {
             stream.get();
          }
          if (stream.peek() == '{')
          {
             if (n == ' ')
             {
                stream.putback(n);
             }
             stream.putback(m);
             multi_element_func(key, stream);
          }
          else
          {
             commonItems::ignoreItem(key, stream);
          }
       });
   this->db_entry_parser_.IgnoreAndLogUnregisteredItems();

   this->registerKeyword("database", this->db_entry_parser_);
   this->IgnoreAndLogUnregisteredItems();
}
}  // namespace vic3