#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <memory>

class Driver;
class Log;
class Config;

class Test : public QObject
{
  Q_OBJECT

private slots:
  void is_config_valid_test_data();
  void is_config_valid_test();

private:
  void set_option(Driver& driver, const std::string& option_name, const std::string& option_value);

  std::shared_ptr<Driver> add_driver(Config& config, const std::string& driver_name, const std::string& driver_type);

  template<typename... Targs>
  std::shared_ptr<Log> add_drivers_to_log(Config& config, Targs&... more);

  template<typename... Targs>
  void add_driver_to_log_recursive(std::shared_ptr<Log>& log, std::shared_ptr<Driver>& driver, Targs&... more);
  void add_driver_to_log_recursive(std::shared_ptr<Log> &) {}
};

#endif  // TEST_H
