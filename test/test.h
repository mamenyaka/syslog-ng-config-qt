#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <memory>

class Object;
class Logpath;
class Config;

class Test : public QObject
{
  Q_OBJECT

private slots:
  void is_config_valid_test_data();
  void is_config_valid_test();

private:
  void set_option(Object& object, const std::string& option_name, const std::string& option_value);

  std::shared_ptr<Object> add_object(Config& config, const std::string& object_name, const std::string& object_type);

  template<typename... Targs>
  std::shared_ptr<Logpath> add_objects_to_logpath(Config& config, Targs&... more);

  template<typename... Targs>
  void add_object_to_logpath_recursive(std::shared_ptr<Logpath>& logpath, std::shared_ptr<Object>& object, Targs&... more);
  void add_object_to_logpath_recursive(std::shared_ptr<Logpath> &) {}
};

#endif  // TEST_H
