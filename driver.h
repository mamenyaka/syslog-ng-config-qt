#ifndef DRIVER_H
#define DRIVER_H

#include "option.h"

#include <string>
#include <vector>

class DefaultDriver
{
public:
  DefaultDriver(const std::string& name, const std::string& type, const std::string& description) :
    name(name),
    type(type),
    description(description)
  {}

  const std::string& get_name() const
  {
    return name;
  }

  const std::string get_type() const
  {
    return type;
  }

  const std::string& get_description() const
  {
    return description;
  }

  std::vector<Option>& get_options()
  {
    return options;
  }

  void add_option(const Option& option)
  {
    options.push_back(std::move(option));
  }

protected:
  std::string name, type, description;
  std::vector<Option> options;
};

class Driver : public DefaultDriver
{
  std::string id;

public:
  Driver(const DefaultDriver& default_driver) :
    DefaultDriver(default_driver)
  {}

  const std::string& get_id() const
  {
    return id;
  }

  void set_id(const int count)
  {
    id = type.substr(0, 1) + "_" + name + std::to_string(count);
  }
};

#endif  // DRIVER_H
