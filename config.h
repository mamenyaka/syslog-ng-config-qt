#ifndef DRIVER_H
#define DRIVER_H

#include <QPoint>

#include <string>
#include <vector>
#include <list>
#include <algorithm>

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
    values.push_back(std::move(value));
  }

  void set_current_value(const std::string& current_value)
  {
    this->current_value = current_value;
  }
};

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

  const std::string print_id() const
  {
    return name + "_" + std::to_string(id);
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
};

class Log
{
  int id;
  QPoint location;
  std::list<Driver*> drivers;

public:
  Log(const int id) :
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

  const std::list<Driver*>& get_drivers() const
  {
    return drivers;
  }

  void set_id(const int id)
  {
    this->id = id;
  }

  void set_location(const QPoint& location)
  {
    this->location = location;
  }

  bool has_driver(Driver* const driver) const
  {
    return drivers.cend() != std::find_if(drivers.cbegin(), drivers.cend(),
                                          [driver](const Driver* d)->bool {
                                            return d == driver;
                                          });
  }

  void add_driver(Driver* const driver)
  {
    drivers.push_back(driver);
  }

  void remove_driver(Driver* const driver)
  {
    drivers.remove(driver);
  }
};

class Config
{
  std::vector<DefaultDriver> default_drivers;
  std::list<Driver> drivers;
  std::list<Log> logs;

public:
  Config()
  {}

  const std::vector<DefaultDriver>& get_default_drivers() const
  {
    return default_drivers;
  }

  std::list<Driver>& get_drivers()
  {
    return drivers;
  }

  std::list<Log>& get_logs()
  {
    return logs;
  }

  void add_default_driver(const DefaultDriver& driver)
  {
    default_drivers.push_back(std::move(driver));
  }

  Driver* add_driver(const Driver& driver)
  {
    drivers.push_back(std::move(driver));
    return &drivers.back();
  }

  Log* add_new_log()
  {
    logs.push_back(Log(logs.size()));
    return &logs.back();
  }

  void erase_driver(Driver* const driver)
  {
    for (Driver& d : drivers)
    {
      if (d.get_name() == driver->get_name() &&
        d.get_type() == driver->get_type() &&
        d.get_id() > driver->get_id())
      {
        d.set_id(d.get_id() - 1);
      }
    }

    for (Log& log : logs)
    {
      if (log.has_driver(driver))
      {
        log.remove_driver(driver);
      }
    }

    drivers.remove_if([driver](const Driver& d)->bool {
                        return &d == driver;
                      });
  }

  void erase_log(Log* const log)
  {
    for (Log& l : logs)
    {
      if (l.get_id() > log->get_id())
      {
        l.set_id(l.get_id() - 1);
      }
    }

    logs.remove_if([log](const Log& l)->bool {
                     return &l == log;
                   });
  }

  void erase_all()
  {
    logs.clear();
    drivers.clear();
  }
};

#endif  // DRIVER_H
