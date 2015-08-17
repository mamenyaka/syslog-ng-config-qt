#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <memory>

class Object;
class ObjectStatement;
class LogStatement;
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
  std::shared_ptr<ObjectStatement> add_object_statement(Config& config, const std::string& object_statement_name);

  template<typename... Targs>
  void add_objects_to_log_statement(Config& config, Targs&... more);

  template<typename... Targs>
  void add_object_to_log_statement_recursive(std::shared_ptr<LogStatement>& log_statement,
                                             std::shared_ptr<ObjectStatement>& object_statement, Targs&... more);
  void add_object_to_log_statement_recursive(std::shared_ptr<LogStatement> &) {}
};

#endif  // TEST_H
