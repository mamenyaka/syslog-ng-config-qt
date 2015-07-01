#include "config.h"

#include <map>
#include <algorithm>

Option::Option(const std::string& name, const std::string& type, const std::string& description, const std::string& default_value) :
  name(name),
  description(description),
  default_value(default_value)
{
  std::map<std::string, OptionType> option_type_map {
    {"string", OptionType::string},
    {"number", OptionType::number},
    {"list", OptionType::list},
    {"set", OptionType::set}
  };

  this->type = option_type_map[type];
}

OptionType Option::get_type() const
{
  return type;
}

const std::string& Option::get_name() const
{
  return name;
}

const std::string& Option::get_description() const
{
  return description;
}

const std::vector<std::string>& Option::get_values() const
{
  return values;
}

const std::string& Option::get_default_value() const
{
  return default_value;
}

const std::string& Option::get_current_value() const
{
  return current_value;
}

void Option::add_value(const std::string& value)
{
  values.push_back(std::move(value));
}

void Option::set_current_value(const std::string& current_value)
{
  this->current_value = current_value;
}

const std::string Option::to_string() const
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

DefaultDriver::DefaultDriver(const std::string& name, const std::string& type, const std::string& description) :
  name(name),
  type(type),
  description(description)
{}

const std::string& DefaultDriver::get_name() const
{
  return name;
}

const std::string& DefaultDriver::get_type() const
{
  return type;
}

const std::string& DefaultDriver::get_description() const
{
  return description;
}

const std::vector<Option>& DefaultDriver::get_options() const
{
  return options;
}

void DefaultDriver::add_option(const Option& option)
{
  options.push_back(std::move(option));
}


Driver::Driver(const DefaultDriver& default_driver, const int id) :
  DefaultDriver(default_driver),
  id(id),
  location(QPoint(50, 50)),
  default_driver(default_driver)
{
  restore_defaults();
}

int Driver::get_id() const
{
  return id;
}

const std::string Driver::print_id() const
{
  return std::string(1, type.at(0)) + "_" + name + std::to_string(id);
}

const QPoint& Driver::get_location() const
{
  return location;
}

const DefaultDriver& Driver::get_default_driver() const
{
  return default_driver;
}

std::vector<Option>& Driver::get_options()
{
  return options;
}

void Driver::set_id(const int id)
{
  this->id = id;
}

void Driver::set_location(const QPoint& location)
{
  this->location = location;
}

void Driver::restore_defaults()
{
  auto options_it = options.begin();
  for (const Option& default_option : default_driver.get_options())
  {
    Option& option = *options_it++;
    option.set_current_value(default_option.get_default_value());
  }
}

const std::string Driver::to_string() const
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

Log::Log() :
  location(QPoint(50, 50))
{}

const QPoint& Log::get_location() const
{
  return location;
}

const std::list<Driver*>& Log::get_drivers() const
{
  return drivers;
}

void Log::set_location(const QPoint& location)
{
  this->location = location;
}

bool Log::has_driver(Driver* const driver) const
{
  return drivers.cend() != std::find_if(drivers.cbegin(), drivers.cend(),
  [driver](const Driver* d)->bool {
    return d == driver;
  });
}

void Log::add_driver(Driver* const driver)
{
  drivers.push_back(driver);
}

void Log::remove_driver(Driver* const driver)
{
  drivers.remove(driver);
}

const std::string Log::to_string() const
{
  std::string config = "log { ";
  for (Driver* const driver : drivers)
  {
    config += driver->get_type() + "(" + driver->print_id() + "); ";
  }
  config += "};\n";

  return config;
}


Config::Config()
{}

const std::vector<DefaultDriver>& Config::get_default_drivers() const
{
  return default_drivers;
}

std::list<Driver>& Config::get_drivers()
{
  return drivers;
}

std::list<Log>& Config::get_logs()
{
  return logs;
}

void Config::add_default_driver(const DefaultDriver& driver)
{
  default_drivers.push_back(std::move(driver));
}

Driver* Config::add_driver(const Driver& driver)
{
  drivers.push_back(std::move(driver));
  return &drivers.back();
}

Log* Config::add_log(const Log& log)
{
  logs.push_back(std::move(log));
  return &logs.back();
}

void Config::delete_driver(Driver* const driver)
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

void Config::delete_log(Log* const log)
{
  logs.remove_if([log](const Log& l)->bool {
    return &l == log;
  });
}

void Config::erase_all()
{
  logs.clear();
  drivers.clear();
}
