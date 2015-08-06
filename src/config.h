#ifndef CONFIG_H
#define CONFIG_H

#include "driver.h"

class Config
{
  std::vector< std::unique_ptr<const Driver> > default_drivers;
  std::list< std::unique_ptr<Driver> > drivers;

  std::list<Log> logs;

  std::unique_ptr<GlobalOptions> global_options;

public:
  Config(const std::string& dir_name = "drivers");

  const std::vector< std::unique_ptr<const Driver> >& get_default_drivers() const;
  const Driver& get_default_driver(const std::string& name, const std::string& type) const;
  std::unique_ptr<GlobalOptions>& get_global_options();

  std::shared_ptr<Driver> add_driver(std::unique_ptr<Driver>& new_driver);
  std::shared_ptr<Log> add_log(std::unique_ptr<Log>& new_log);

  void parse_yaml(const std::string& file_name);

  const std::string to_string() const;

private:
  void update_drivers_id(const std::string& name, const std::string& type);
  void delete_driver(const Driver* old_driver);
  void delete_log(const Log* old_log);
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif  // CONFIG_H
