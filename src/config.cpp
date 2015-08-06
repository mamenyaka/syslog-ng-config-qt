#include "config.h"

#include <QDirIterator>

#include <yaml-cpp/yaml.h>

#include <map>

enum class OptionType : int { string, number, list, set, tls, value_pairs };

std::map<std::string, OptionType> option_type_map {
  {"string", OptionType::string},
  {"number", OptionType::number},
  {"list", OptionType::list},
  {"set", OptionType::set},
  {"tls", OptionType::tls},
  {"value-pairs", OptionType::value_pairs}
};

enum class DriverType : int { source, destination, filter, template_, rewrite, parser, options };

std::map<std::string, DriverType> driver_type_map {
  {"source", DriverType::source},
  {"destination", DriverType::destination},
  {"options", DriverType::options},
  {"filter", DriverType::filter},
  {"template", DriverType::template_},
  {"rewrite", DriverType::rewrite},
  {"parser", DriverType::parser}
};


Config::Config(const std::string& dir_name)
{
  default_drivers.reserve(60);

  QDirIterator it(QString::fromStdString(dir_name));
  while (it.hasNext())
  {
    it.next();
    if (!it.fileInfo().isFile())
    {
      continue;
    }

    parse_yaml(it.filePath().toStdString());
  }

  std::sort(default_drivers.begin(), default_drivers.end(),
            [](std::unique_ptr<const Driver>& a, std::unique_ptr<const Driver>& b)->bool {
              return a->get_name() < b->get_name();
            });

  const Options& options_global = static_cast<const Options&>(get_default_driver("global", "options"));
  const Options& options_tls = static_cast<const Options&>(get_default_driver("tls", "options"));
  const Options& options_value_pairs = static_cast<const Options&>(get_default_driver("value-pairs", "options"));

  global_options = std::make_unique<GlobalOptions>(options_global);

  for (std::unique_ptr<const Driver>& driver : default_drivers)
  {
    for (const std::unique_ptr<Option>& option : driver->get_options())
    {
      if (option->get_name() == "tls")
      {
        static_cast<ExternOption&>(*option).set_options(options_tls);
        break;
      }

      if (option->get_name() == "value-pairs")
      {
        static_cast<ExternOption&>(*option).set_options(options_value_pairs);
        break;
      }
    }
  }
}

const std::vector< std::unique_ptr<const Driver> >& Config::get_default_drivers() const
{
  return default_drivers;
}

const Driver& Config::get_default_driver(const std::string& name, const std::string& type) const
{
  auto it = std::find_if(default_drivers.cbegin(), default_drivers.cend(),
                         [&name, &type](const std::unique_ptr<const Driver>& driver)->bool {
                           return driver->get_name() == name && driver->get_type() == type;
                         });
  return **it;
}

std::unique_ptr<GlobalOptions>& Config::get_global_options()
{
  return global_options;
}

std::shared_ptr<Driver> Config::add_driver(std::unique_ptr<Driver>& new_driver)
{
  const std::string name = new_driver->get_name();
  const std::string type = new_driver->get_type();

  drivers.push_back(std::move(new_driver));

  update_drivers_id(name, type);

  std::unique_ptr<Driver>& driver = drivers.back();

  return std::shared_ptr<Driver>(driver.get(), [&](const Driver* driver) { delete_driver(driver); });
}

std::shared_ptr<Log> Config::add_log(std::unique_ptr<Log>& new_log)
{
  logs.push_back(std::move(*new_log));

  Log& log = logs.back();

  return std::shared_ptr<Log>(&log, [&](const Log* log) { delete_log(log); });
}

void Config::parse_yaml(const std::string& file_name)
{
  const YAML::Node yaml_driver = YAML::LoadFile(file_name);

  const std::string name = yaml_driver["name"].as<std::string>();
  const std::string type = yaml_driver["type"].as<std::string>();
  const std::string description = yaml_driver["description"].as<std::string>();

  Driver* driver;
  DriverType driver_type = driver_type_map[type];
  switch (driver_type)
  {
    case DriverType::source:
      driver = new Source(name, description);
      break;
    case DriverType::destination:
      driver = new Destination(name, description);
      break;
    case DriverType::options:
      driver = new Options(name, description);
      break;
    case DriverType::filter:
      driver = new Filter(name, description);
      break;
    case DriverType::template_:
      driver = new Template(name, description);
      break;
    case DriverType::rewrite:
      driver = new Rewrite(name, description);
      break;
    case DriverType::parser:
      driver = new Parser(name, description);
      break;
  }

  if (yaml_driver["include"])
  {
    driver->set_include(yaml_driver["include"].as<std::string>());
  }

  const YAML::Node& options = yaml_driver["options"];
  for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
  {
    YAML::const_iterator tmp = option_it->begin();
    const YAML::Node& yaml_option = tmp->second;

    const std::string name = tmp->first.as<std::string>();
    const std::string type = yaml_option["type"].as<std::string>();
    const std::string description = yaml_option["description"].as<std::string>();

    Option* option;
    OptionType option_type = option_type_map[type];
    switch (option_type)
    {
      case OptionType::string:
        option = new StringOption(name, description);
        break;
      case OptionType::number:
        option = new NumberOption(name, description);
        break;
      case OptionType::list:
        option = new ListOption(name, description);
        break;
      case OptionType::set:
        option = new SetOption(name, description);
        break;
      case OptionType::tls:
      case OptionType::value_pairs:
        option = new ExternOption(name, description);
        break;
    }

    const YAML::Node& values = yaml_option["values"];
    for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
    {
      const std::string value = value_it->as<std::string>();

      if (option_type == OptionType::list)
      {
        static_cast<ListOption*>(option)->add_value(value);
      }
      else if (option_type == OptionType::set)
      {
        static_cast<SetOption*>(option)->add_value(value);
      }
    }

    if (yaml_option["default"])
    {
      option->set_default(yaml_option["default"].as<std::string>());
    }

    if (yaml_option["required"])
    {
      option->set_required(yaml_option["required"].as<bool>());
    }

    driver->add_option(option);
  }

  default_drivers.emplace_back(driver);
}

const std::string Config::to_string() const
{
  std::string config;

  config += global_options->to_string();

  for (const std::unique_ptr<Driver>& driver : drivers)
  {
    config += driver->to_string();
  }

  for (const Log& log : logs)
  {
    config += log.to_string();
  }

  return config;
}

void Config::update_drivers_id(const std::string& name, const std::string& type)
{
  int id = 0;
  for (std::unique_ptr<Driver>& driver : drivers)
  {
    if (driver->get_name() == name && driver->get_type() == type)
    {
      driver->set_id(id++);
    }
  }
}

void Config::delete_driver(const Driver* old_driver)
{
  const std::string name = old_driver->get_name();
  const std::string type = old_driver->get_type();

  drivers.remove_if([old_driver](std::unique_ptr<Driver>& driver)->bool { return driver.get() == old_driver; });

  update_drivers_id(name, type);
}

void Config::delete_log(const Log* old_log)
{
  logs.remove_if([old_log](const Log& log)->bool { return &log == old_log; });
}

std::ostream& operator<<(std::ostream& os, const Config& config)
{
  os << config.to_string();

  return os;
}
