#ifndef CONFIG_H
#define CONFIG_H

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
  std::string default_value, current_value;

public:
  Option(const std::string& name, const std::string& type, const std::string& description, const std::string& default_value) :
    name(name),
    description(description),
    default_value(default_value)
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

  const std::string& get_default_value() const
  {
    return default_value;
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

  const std::string to_string() const
  {
    if (current_value == default_value)
    {
      return "";
    }

    std::string config = "\t\t" + name + "(";

    if (type == OptionType::number ||
      (type == OptionType::list && current_value.cend() == std::find_if_not(current_value.cbegin(), current_value.cend(), isdigit)))
    {
      config += current_value;
    }
    else
    {
      config += "\"" + current_value + "\"";
    }

    config += ")\n";

    return config;
  }
};

class DefaultDriver
{
protected:
  std::string name, type, description;
  std::vector<Option> options;

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

  const std::vector<Option>& get_options() const
  {
    return options;
  }

  void add_option(const Option& option)
  {
    options.push_back(std::move(option));
  }
};

class Driver : public DefaultDriver
{
  int id;
  QPoint location;
  const DefaultDriver& default_driver;

public:
  Driver(const DefaultDriver& default_driver, const int id) :
    DefaultDriver(default_driver),
    id(id),
    location(QPoint(50, 50)),
    default_driver(default_driver)
  {
    restore_defaults();
  }

  int get_id() const
  {
    return id;
  }

  const std::string print_id() const
  {
    return std::string(1, type.at(0)) + "_" + name + std::to_string(id);
  }

  const QPoint& get_location() const
  {
    return location;
  }

  const DefaultDriver& get_default_driver() const
  {
    return default_driver;
  }

  std::vector<Option>& get_options()
  {
    return options;
  }

  void set_id(const int id)
  {
    this->id = id;
  }

  void set_location(const QPoint& location)
  {
    this->location = location;
  }

  void restore_defaults()
  {
    auto options_it = options.begin();
    for (const Option& default_option : default_driver.get_options())
    {
      Option& option = *options_it++;
      option.set_current_value(default_option.get_default_value());
    }
  }

  const std::string to_string() const
  {
    std::string config = type + " " + print_id() + " {\n";

    config += "\t" + name + "(\n";
    for (const Option& option : options)
    {
      config += option.to_string();
    }
    config += "\t);\n};\n";

    return config;
  }
};

class Log
{
  QPoint location;
  std::list<Driver*> drivers;

public:
  Log() :
    location(QPoint(50, 50))
  {}

  const QPoint& get_location() const
  {
    return location;
  }

  const std::list<Driver*>& get_drivers() const
  {
    return drivers;
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

  const std::string to_string() const
  {
    std::string config = "log { ";
    for (Driver* const driver : drivers)
    {
      config += driver->get_type() + "(" + driver->print_id() + "); ";
    }
    config += "};\n";

    return config;
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

  Log* add_log(const Log& log)
  {
    logs.push_back(std::move(log));
    return &logs.back();
  }

  void delete_driver(Driver* const driver)
  {
    for (Log& log : logs)
    {
      if (log.has_driver(driver))
      {
        log.remove_driver(driver);
      }
    }

    for (Driver& d : drivers)
    {
      if (d.get_name() == driver->get_name() &&
        d.get_type() == driver->get_type() &&
        d.get_id() > driver->get_id())
      {
        d.set_id(d.get_id() - 1);
      }
    }

    drivers.remove_if([driver](const Driver& d)->bool {
                        return &d == driver;
                      });
  }

  void delete_log(Log* const log)
  {
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

#endif  // CONFIG_H
