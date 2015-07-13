#ifndef CONFIG_H
#define CONFIG_H

#include <QPoint>

#include <string>
#include <vector>
#include <list>

enum class OptionType { string, number, list, set, tls, value_pairs };

class DefaultOption
{
  OptionType type;
  std::string name, description;
  std::vector<std::string> values;
  std::string default_value;
  bool required = false;

public:
  DefaultOption(const std::string& name,
                const std::string& type,
                const std::string& description,
                const std::string& default_value);

  OptionType get_type() const;
  const std::string& get_name() const;
  const std::string& get_description() const;
  const std::vector<std::string>& get_values() const;
  const std::string& get_default_value() const;
  bool is_required() const;

  void add_value(const std::string& value);
  void set_required(const bool required);
};

class DefaultDriver
{
  std::string name, type, description;
  std::string include;
  std::vector<DefaultOption> default_options;

public:
  DefaultDriver(const std::string& name,
                const std::string& type,
                const std::string& description);

  const std::string& get_name() const;
  const std::string& get_type() const;
  const std::string& get_description() const;
  const std::string& get_include() const;
  const std::vector<DefaultOption>& get_default_options() const;

  void set_include(const std::string& include);
  void add_option(const DefaultOption& option);
};

class Option : public DefaultOption
{
  std::string current_value;

public:
  Option(const DefaultOption& default_option);

  const std::string& get_current_value() const;

  void add_value(const std::string& value) = delete;
  void set_required(const bool required) = delete;

  void set_current_value(const std::string& current_value);

  const std::string to_string() const;
};

class Driver : public DefaultDriver
{
  std::vector<Option> options;
  int id;
  QPoint location;

public:
  Driver(const DefaultDriver& default_driver,
         const int id,
         QPoint location = QPoint(50, 50));

  std::vector<Option>& get_options();
  const std::vector<Option>& get_options() const;
  const std::string get_id() const;
  const QPoint& get_location() const;

  const std::vector<DefaultOption>& get_default_options() const = delete;
  void set_include(const std::string& include) = delete;
  void add_option(const DefaultOption& option) = delete;

  void restore_defaults();
  void update_id(const int id);
  void update_location(const QPoint& location);

  const std::string to_string() const;
};

class GlobalOptions : public Driver
{
public:
  GlobalOptions(const DefaultDriver& driver);

  const std::string& get_include() const = delete;
  const std::string get_id() const = delete;
  const QPoint& get_location() const = delete;

  void update_id(const int id) = delete;
  void update_location(const QPoint& location) = delete;
};

class Log
{
  std::list<Driver*> drivers;
  QPoint location;

public:
  Log();

  const std::list<Driver*>& get_drivers() const;
  bool has_driver(Driver* const driver) const;
  const QPoint& get_location() const;

  void add_driver(Driver* const driver);
  void remove_driver(Driver* const driver);
  void update_location(const QPoint& location);

  const std::string to_string() const;
};

class Config
{
  std::vector<DefaultDriver> default_drivers;
  std::list<Driver> drivers;
  std::list<Log> logs;
  GlobalOptions* global_options;

public:
  Config(const std::string& dir_name = "drivers");
  ~Config();

  const std::vector<DefaultDriver>& get_default_drivers() const;
  const DefaultDriver& get_default_driver(const std::string& name, const std::string& type) const;
  std::list<Driver>& get_drivers();
  const std::list<Driver>& get_drivers() const;
  std::list<Log>& get_logs();
  const std::list<Log>& get_logs() const;
  GlobalOptions& get_global_options();

  void add_driver(const Driver& driver);
  void add_log(const Log& log);
  void delete_driver(Driver* driver);
  void delete_log(Log* log);

  void parse_yaml(const std::string& file_name);
  void erase_all();
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif  // CONFIG_H
