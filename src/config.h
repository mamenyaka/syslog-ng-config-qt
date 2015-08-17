#ifndef CONFIG_H
#define CONFIG_H

#include "object.h"

class Config
{
  std::vector< std::unique_ptr<const Object> > default_objects;

  std::unique_ptr<GlobalOptions> global_options;
  std::list< std::unique_ptr<ObjectStatement> > object_statements;
  std::list< std::unique_ptr<LogStatement> > log_statements;

public:
  Config(const std::string& dir_name = "objects");
  ~Config();

  const std::vector< std::unique_ptr<const Object> >& get_default_objects() const;
  const Object& get_default_object(const std::string& name, const std::string& type) const;

  Options& get_global_options();
  const std::list< std::unique_ptr<ObjectStatement> >& get_object_statements() const;
  const std::list< std::unique_ptr<LogStatement> >& get_log_statements() const;

  std::shared_ptr<ObjectStatement> add_object_statement(ObjectStatement* new_object_statement);
  std::shared_ptr<LogStatement> add_log_statement(LogStatement* new_log_statement);

  void parse_yaml(const std::string& file_name);

  const std::string to_string() const;

private:
  void delete_object_statement(const ObjectStatement* old_object_statement);
  void delete_log_statement(const LogStatement* old_log_statement);

  bool deleted = false;
};

#endif  // CONFIG_H
