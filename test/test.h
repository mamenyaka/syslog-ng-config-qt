#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <memory>

class Driver;
class Log;
class Config;

typedef std::unique_ptr< Log, std::function<void(const Log *)> > LogUPtr;

class Test : public QObject
{
  Q_OBJECT

private slots:
  void is_config_valid_test_data();
  void is_config_valid_test();

private:
  void set_option(Driver& driver, const std::string& option_name, const std::string& option_value);

  template<typename... Targs>
  LogUPtr add_drivers_to_log(Config& config, Targs&... more);

  template<typename... Targs>
  void add_driver_to_log_recursive(LogUPtr& log, std::shared_ptr<Driver>& driver, Targs&... more);
  void add_driver_to_log_recursive(LogUPtr& log);
};

#endif  // TEST_H
