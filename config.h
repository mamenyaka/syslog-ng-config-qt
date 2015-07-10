#ifndef CONFIG_H
#define CONFIG_H

#include <QPoint>

#include <string>
#include <vector>
#include <list>

enum class OptionType { string, number, list, set };

class Option
{
  OptionType type;
  std::string name, description;
  std::vector<std::string> values;
  std::string default_value, current_value;
  bool required = false;

public:
  Option(const std::string& name, const std::string& type, const std::string& description, const std::string& default_value);

  OptionType get_type() const;
  const std::string& get_name() const;
  const std::string& get_description() const;
  const std::vector<std::string>& get_values() const;
  const std::string& get_default_value() const;
  const std::string& get_current_value() const;
  bool is_required() const;

  void add_value(const std::string& value);
  void set_current_value(const std::string& current_value);
  void set_required();

  const std::string to_string() const;
};

class DefaultDriver
{
protected:
  std::string name, type, description;
  std::vector<Option> options;

public:
  DefaultDriver(const std::string& name, const std::string& type, const std::string& description);

  const std::string& get_name() const;
  const std::string& get_type() const;
  const std::string& get_description() const;
  const std::vector<Option>& get_options() const;

  void add_option(const Option& option);
};

class Driver : public DefaultDriver
{
  int id;
  QPoint location;
  const DefaultDriver& default_driver;

public:
  Driver(const DefaultDriver& default_driver, const int id);

  const std::string get_id() const;
  const QPoint& get_location() const;
  const DefaultDriver& get_default_driver() const;
  std::vector<Option>& get_options();

  void update_id(const int id);
  void update_location(const QPoint& location);
  void restore_defaults();
  void add_option(const Option& option) = delete;

  const std::string to_string() const;
};

class Log
{
  QPoint location;
  std::list<Driver*> drivers;

public:
  Log();

  const QPoint& get_location() const;
  const std::list<Driver*>& get_drivers() const;
  bool has_driver(Driver* const driver) const;

  void update_location(const QPoint& location);
  void add_driver(Driver* const driver);
  void remove_driver(Driver* const driver);

  const std::string to_string() const;
};

class Config
{
  std::vector<DefaultDriver> default_drivers;
  std::list<Driver> drivers;
  std::list<Log> logs;

public:
  Config();
  Config(const std::string& file_name);

  const std::vector<DefaultDriver>& get_default_drivers() const;
  std::list<Driver>& get_drivers();
  const std::list<Driver>& get_drivers() const;
  std::list<Log>& get_logs();
  const std::list<Log>& get_logs() const;

  void add_driver(const Driver& driver);
  void add_log(const Log& log);
  void delete_driver(Driver* driver);
  void delete_log(Log* log);

  void parse_yaml(const std::string& file_name);
  void erase_all();
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif  // CONFIG_H
