#ifndef DRIVER_H
#define DRIVER_H

#include "option.h"

#include <QPoint>

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

  const std::string& get_type() const
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
  int id;
  QPoint location;

public:
  Driver(const DefaultDriver& default_driver, const int id) :
    DefaultDriver(default_driver),
    id(id)
  {}

  int get_id() const
  {
    return id;
  }

  const QPoint& get_location() const
  {
    return location;
  }

  void set_id(const int id)
  {
    this->id = id;
  }

  void set_location(const QPoint& location)
  {
    this->location = location;
  }

  bool operator==(const Driver& driver)
  {
    return name == driver.get_name() && type == driver.get_type() && id == driver.get_id();
  }
};

#endif  // DRIVER_H
