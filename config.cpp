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

bool Option::has_default_value() const
{
  return default_value != "";
}

const std::string Option::to_string() const
{
  std::string config = name + "(";

  if (type == OptionType::number)
  {
    config += current_value;
  }
  else
  {
    config += "\"" + current_value + "\"";
  }

  config += ")";

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

const std::string Driver::get_id() const
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

void Driver::update_id(const int id)
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
  std::string config = type + " " + get_id() + " {\n";

  config += "    " + name + "(";

  for (const Option& option : options)
  {
    if (name == option.get_name())
    {
      config += "\"" + option.get_current_value() + "\"";
      break;
    }
  }

  config += "\n";

  for (const Option& option : options)
  {
    if (name == option.get_name() ||
      option.get_current_value() == option.get_default_value())
    {
      continue;
    }

    config += "        " + option.to_string() + "\n";
  }

  config += "    );\n};\n";

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
    config += driver->get_type() + "(" + driver->get_id() + "); ";
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

const std::list< Driver >& Config::get_drivers() const
{
  return drivers;
}

const std::list< Log >& Config::get_logs() const
{
  return logs;
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

void Config::add_driver(const Driver& driver)
{
  drivers.push_back(std::move(driver));
}

void Config::add_log(const Log& log)
{
  logs.push_back(std::move(log));
}

void Config::delete_driver(Driver* driver)
{
  const std::string name = driver->get_name();
  const std::string type = driver->get_type();

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

  driver = nullptr;

  int count = 0;
  for (Driver& driver : drivers)
  {
    if (driver.get_name() == name && driver.get_type() == type)
    {
      driver.update_id(count++);
    }
  }
}

void Config::delete_log(Log* log)
{
  logs.remove_if([log](const Log& l)->bool {
    return &l == log;
  });

  log = nullptr;
}

void Config::erase_all()
{
  logs.clear();
  drivers.clear();
}
