#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>

enum class OptionType { string, number, list, set };

class Option
{
  OptionType type;
  std::string name, description;
  std::vector<std::string> values;
  std::string current_value;

public:
  Option(const std::string& name, const std::string& type, const std::string& description, const std::string& current_value) :
    name(name),
    description(description),
    current_value(current_value)
  {
    if (type == "string")
      this->type = OptionType::string;
    if (type == "number")
      this->type = OptionType::number;
    if (type == "list")
      this->type = OptionType::list;
    if (type == "set")
      this->type = OptionType::set;
  }

  OptionType get_type() const
  {
    return type;
  }
  const std::string& get_name() const
  {
    return name;
  }
  const std::string& get_description() const
  {
    return description;
  }
  const std::vector<std::string>& get_values() const
  {
    return values;
  }
  const std::string& get_current_value() const
  {
    return current_value;
  }

  void add_value(const std::string& value)
  {
    values.push_back(value);
  }
  void set_current_value(const std::string& current_value)
  {
    this->current_value = current_value;
  }
};

#endif  // OPTION_H
