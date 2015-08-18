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

  std::vector< std::shared_ptr<ObjectStatement> > object_statements;
  std::vector< std::shared_ptr<LogStatement> > log_statements;

private slots:
  void is_config_valid_test_data();
  void is_config_valid_test();

private:
  void set_option(Object& object, const std::string& option_name, const std::string& option_value);

  std::shared_ptr<Object> add_object(Config& config, const std::string& object_name, const std::string& object_type);
  std::shared_ptr<ObjectStatement>& add_object_statement(Config& config, const std::string& object_statement_name);

  void add_object_statements_to_log_statement(Config& config, const std::vector<std::string>& object_statement_names);

  std::shared_ptr<ObjectStatement>& find_object_statement(const std::string& name);
};

#endif  // TEST_H
