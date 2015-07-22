#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <functional>

enum class OptionType : int { string, number, list, set, tls, value_pairs };

class Option
{
  std::string name;
  OptionType type;
  std::string description;
  std::vector<std::string> values;
  std::string default_value;
  std::string current_value;
  bool required = false;

public:
  Option(const std::string& name,
         const std::string& type,
         const std::string& description);

  const std::string& get_name() const;
  OptionType get_type() const;
  const std::string& get_description() const;
  const std::vector<std::string>& get_values() const;
  const std::string& get_default_value() const;
  const std::string& get_current_value() const;
  bool is_required() const;

  void add_value(const std::string& value);
  void set_default_value(const std::string& default_value);
  void set_current_value(const std::string& current_value);
  void set_required(bool required);

  const std::string to_string() const;
};

enum class DriverType : int { source, destination, options };

class Driver
{
  std::string name;
  DriverType type;
  std::string description;
  std::string include;
  std::vector<Option> options;
  int id = -1;

public:
  Driver(const std::string& name,
         const std::string& type,
         const std::string& description);

  const std::string& get_name() const;
  DriverType get_type() const;
  const std::string& get_description() const;
  const std::string& get_include() const;
  std::vector<Option>& get_options();
  const std::vector<Option>& get_options() const;
  int get_id() const;

  void set_include(const std::string& include);
  void add_option(const Option& option);
  void update_id(int id);

  void restore_defaults();

  const std::string get_type_name() const;
  const std::string get_id_name() const;
  const std::string to_string() const;
};

class GlobalOptions : public Driver
{
public:
  GlobalOptions(const Driver& driver);

  const std::string to_string() const;

  const std::string& get_include() const = delete;
  int get_id() const = delete;

  void set_include(const std::string& include) = delete;
  void add_option(const Option& option) = delete;
  void update_id(int id) = delete;

  const std::string get_type_name() const = delete;
  const std::string get_id_name() const = delete;
};

class Log
{
  std::list< std::shared_ptr<const Driver> > drivers;

public:
  Log();

  const std::list< std::shared_ptr<const Driver> >& get_drivers() const;

  void add_driver(std::shared_ptr<const Driver>& driver);
  void remove_driver(const std::shared_ptr<const Driver>& driver);

  const std::string to_string() const;
};

class Config
{
  std::vector<Driver> default_drivers;
  std::list<Driver> drivers;
  std::list<Log> logs;
  std::unique_ptr<GlobalOptions> global_options;

public:
  Config(const std::string& dir_name = "drivers");

  const std::vector<Driver>& get_default_drivers() const;
  GlobalOptions& get_global_options();

  std::shared_ptr<Driver> add_driver(const Driver& new_driver);
  std::unique_ptr< Log, std::function<void(const Log *)> > add_log(const Log& new_log);

  const Driver& get_default_driver(const std::string& name, DriverType type) const;

  Driver parse_yaml(const std::string& file_name);

  const std::string to_string() const;

private:
  void delete_driver(const Driver* old_driver);
  void delete_log(const Log* old_log);

  int get_driver_count(const std::string& name, DriverType type) const;
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif  // CONFIG_H
