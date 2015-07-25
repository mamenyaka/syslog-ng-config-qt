#include "config.h"

#include <QDirIterator>

#include <yaml-cpp/yaml.h>

#include <map>
#include <algorithm>

std::map<std::string, OptionType> option_type_map {
  {"string", OptionType::string},
  {"number", OptionType::number},
  {"list", OptionType::list},
  {"set", OptionType::set},
  {"tls", OptionType::tls},
  {"value-pairs", OptionType::value_pairs}
};

Option::Option(const std::string& name,
               const std::string& type,
               const std::string& description) :
  name(name),
  type(option_type_map[type]),
  description(description)
{}

const std::string& Option::get_name() const
{
  return name;
}

OptionType Option::get_type() const
{
  return type;
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

bool Option::is_required() const
{
  return required;
}

void Option::add_value(const std::string& value)
{
  values.push_back(std::move(value));
}

void Option::set_default_value(const std::string& default_value)
{
  current_value = default_value;
  this->default_value = default_value;
}

void Option::set_current_value(const std::string& current_value)
{
  this->current_value = current_value;
}

void Option::set_required(bool required)
{
  this->required = required;
}

const std::string Option::to_string() const
{
  std::string config = name + "(";

  if (type == OptionType::number || type == OptionType::list)
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


std::map<std::string, DriverType> driver_type_map {
  {"source", DriverType::source},
  {"destination", DriverType::destination},
  {"log", DriverType::log},
  {"options", DriverType::options},
  {"filter", DriverType::filter},
  {"template", DriverType::template_},
  {"rewrite", DriverType::rewrite},
  {"parser", DriverType::parser}
};

Driver::Driver(const std::string& name,
               const std::string& type,
               const std::string& description) :
  name(name),
  type(driver_type_map[type]),
  description(description)
{}

const std::string& Driver::get_name() const
{
  return name;
}

DriverType Driver::get_type() const
{
  return type;
}

const std::string& Driver::get_description() const
{
  return description;
}

const std::string& Driver::get_include() const
{
  return include;
}

std::vector<Option>& Driver::get_options()
{
  return options;
}

const std::vector<Option>& Driver::get_options() const
{
  return options;
}

int Driver::get_id() const
{
  return id;
}

void Driver::set_include(const std::string& include)
{
  this->include = include;
}

void Driver::add_option(const Option& option)
{
  options.push_back(std::move(option));
}

void Driver::update_id(int id)
{
  this->id = id;
}

void Driver::restore_defaults()
{
  for (Option& option : options)
  {
    option.set_current_value(option.get_default_value());
  }
}

const std::string Driver::get_type_name() const
{
  return std::find_if(driver_type_map.cbegin(), driver_type_map.cend(),
                      [&](const auto& pair)->bool {
                        return pair.second == type;
                      })->first;
}

const std::string Driver::get_id_name() const
{
  return std::string(1, get_type_name().at(0)) + "_" + name + std::to_string(id);
}

const std::string Driver::to_string() const
{
  std::string config;

  if (!include.empty())
  {
    config += "@include \"" + include + "\"\n\n";
  }

  config += get_type_name() + " " + get_id_name() + " {\n";
  config += "    " + name + "(";

  for (const Option& option : options)
  {
    if (name == option.get_name())
    {
      config += "\"" + option.get_current_value() + "\"";
      if (type == DriverType::rewrite)
      {
        config += ", ";
      }
    }
  }

  config += "\n";

  for (const Option& option : options)
  {
    if (name == option.get_name() ||
      (!option.is_required() && option.get_current_value() == option.get_default_value()))
    {
      continue;
    }

    config += "        " + option.to_string();
    if (type == DriverType::rewrite)
    {
      config += ",";
    }
    config += "\n";
  }

  config += "    );\n};\n";

  return config;
}


Log::Log(const Driver& driver) :
  Driver(driver)
{}

void Log::add_driver(std::shared_ptr<const Driver> driver)
{
  drivers.push_back(std::move(driver));
}

void Log::remove_driver(const std::shared_ptr<const Driver>& driver)
{
  drivers.remove(driver);
}

const std::string Log::to_string() const
{
  std::string config = get_type_name() + " {\n";

  for (const std::shared_ptr<const Driver>& driver : drivers)
  {
    config += "    " + driver->get_type_name() + "(" + driver->get_id_name() + ");\n";
  }

  for (const Option& option : get_options())
  {
    if (option.get_current_value() == option.get_default_value())
    {
      continue;
    }

    config += "    " + option.to_string() + ";\n";
  }

  config += "};\n";

  return config;
}


GlobalOptions::GlobalOptions(const Driver& default_driver) :
  Driver(default_driver)
{}

const std::string GlobalOptions::to_string() const
{
  std::string config = get_type_name() + " {\n";

  for (const Option& option : get_options())
  {
    if (option.get_current_value() == option.get_default_value())
    {
      continue;
    }

    config += "    " + option.to_string() + ";\n";
  }

  config += "};\n";

  return config;
}


Config::Config(const std::string& dir_name)
{
  QDirIterator it(QString::fromStdString(dir_name));
  while (it.hasNext())
  {
    it.next();
    if (!it.fileInfo().isFile())
    {
      continue;
    }

    Driver driver = parse_yaml(it.filePath().toStdString());

    if (driver.get_type() == DriverType::options)
    {
      global_options = std::make_unique<GlobalOptions>(driver);
    }
    else
    {
      default_drivers.push_back(std::move(driver));
    }
  }

  std::sort(default_drivers.begin(), default_drivers.end(),
            [](const Driver& a, const Driver& b)->bool {
              return a.get_name() < b.get_name();
            });
}

const std::vector<Driver>& Config::get_default_drivers() const
{
  return default_drivers;
}

GlobalOptions& Config::get_global_options()
{
  return *global_options;
}

std::shared_ptr<Driver> Config::add_driver(const Driver& new_driver)
{
  const int id = get_driver_count(new_driver.get_name(), new_driver.get_type());

  drivers.push_back(std::move(new_driver));

  Driver& driver = drivers.back();
  driver.update_id(id);

  return std::shared_ptr<Driver>(&driver, [&](const Driver* driver) { delete_driver(driver); });
}

Config::LogUPtr Config::add_log(const Log& new_log)
{
  logs.push_back(std::move(new_log));

  Log& log = logs.back();

  return LogUPtr(&log, [&](const Log* log) { delete_log(log); });
}

const Driver& Config::get_default_driver(const std::string& name, DriverType type) const
{
  return *std::find_if(default_drivers.cbegin(), default_drivers.cend(),
                       [&name, type](const Driver& driver)->bool {
                         return driver.get_name() == name && driver.get_type() == type;
                       });
}

Driver Config::parse_yaml(const std::string& file_name)
{
  const YAML::Node yaml_driver = YAML::LoadFile(file_name);

  const std::string name = yaml_driver["name"].as<std::string>();
  const std::string type = yaml_driver["type"].as<std::string>();
  const std::string description = yaml_driver["description"].as<std::string>();

  Driver driver(name, type, description);

  if (yaml_driver["include"])
  {
    driver.set_include(yaml_driver["include"].as<std::string>());
  }

  const YAML::Node& options = yaml_driver["options"];
  for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
  {
    YAML::const_iterator tmp = option_it->begin();
    const YAML::Node& yaml_option = tmp->second;

    const std::string name = tmp->first.as<std::string>();
    const std::string type = yaml_option["type"].as<std::string>();
    const std::string description = yaml_option["description"].as<std::string>();

    Option option(name, type, description);

    const YAML::Node& values = yaml_option["values"];
    for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
    {
      const std::string value = value_it->as<std::string>();
      option.add_value(value);
    }

    if (yaml_option["default"])
    {
      option.set_default_value(yaml_option["default"].as<std::string>());
    }

    if (yaml_option["required"])
    {
      option.set_required(yaml_option["required"].as<bool>());
    }

    driver.add_option(option);
  }

  return driver;
}

const std::string Config::to_string() const
{
  std::string config;
  config += global_options->to_string() + "\n";

  for (const Driver& driver : drivers)
  {
    config += driver.to_string() + "\n";
  }

  for (const Log& log : logs)
  {
    config += log.to_string() + "\n";
  }

  return config;
}

int Config::get_driver_count(const std::string& name, DriverType type) const
{
  return std::count_if(drivers.cbegin(), drivers.cend(),
                       [&name, type](const Driver& driver)->bool {
                         return driver.get_name() == name && driver.get_type() == type;
                      });
}

void Config::delete_driver(const Driver* old_driver)
{
  const std::string name = old_driver->get_name();
  DriverType type = old_driver->get_type();

  drivers.remove_if([old_driver](const Driver& driver)->bool {
    return &driver == old_driver;
  });

  int id = 0;
  for (Driver& driver : drivers)
  {
    if (driver.get_name() == name && driver.get_type() == type)
    {
      driver.update_id(id++);
    }
  }
}

void Config::delete_log(const Log* old_log)
{
  logs.remove_if([old_log](const Log& log)->bool {
    return &log == old_log;
  });
}

std::ostream& operator<<(std::ostream& os, const Config& config)
{
  os << config.to_string();

  return os;
}
