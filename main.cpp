#include "mainwindow.h"
#include "config.h"

#include <QApplication>

#include <yaml-cpp/yaml.h>

void parse_yaml(Config& config)
{
  const YAML::Node root = YAML::LoadFile("drivers.yml");
  for (YAML::const_iterator driver_it = root.begin(); driver_it != root.end(); ++driver_it)
  {
    const YAML::Node& yaml_driver = driver_it->second;

    const std::string name = yaml_driver["name"].as<std::string>();
    const std::string type = yaml_driver["type"].as<std::string>();
    const std::string description = yaml_driver["description"] ? yaml_driver["description"].as<std::string>() : "";
    const YAML::Node& options = yaml_driver["options"];

    DefaultDriver driver(name, type, description);

    for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
    {
      YAML::const_iterator tmp = option_it->begin();
      const YAML::Node& yaml_option = tmp->second;

      const std::string name = tmp->first.as<std::string>();
      const std::string type = yaml_option["type"].as<std::string>();
      const std::string description = yaml_option["description"] ? yaml_option["description"].as<std::string>() : "";
      const std::string default_value = yaml_option["default"] ? yaml_option["default"].as<std::string>() : "";

      Option option(name, type, description, default_value);

      if (type == "list" || type == "set")
      {
        const YAML::Node& values = yaml_option["values"];
        for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
        {
          const std::string value = value_it->as<std::string>();
          option.add_value(value);
        }
      }

      driver.add_option(option);
    }

    config.add_default_driver(driver);
  }
}

int main(int argc, char *argv[])
{
  Config config;
  parse_yaml(config);

  QApplication a(argc, argv);
  MainWindow w(config);
  w.show();

  return a.exec();
}
