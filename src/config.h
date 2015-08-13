#ifndef CONFIG_H
#define CONFIG_H

#include "object.h"

class Config
{
  std::vector< std::unique_ptr<const Object> > default_objects;
  std::list< std::unique_ptr<Object> > objects;

  std::list< std::unique_ptr<Logpath> > logpaths;

  std::unique_ptr<GlobalOptions> global_options;

public:
  Config(const std::string& dir_name = "objects");
  ~Config();

  const std::vector< std::unique_ptr<const Object> >& get_default_objects() const;
  const Object& get_default_object(const std::string& name, const std::string& type) const;
  std::unique_ptr<GlobalOptions>& get_global_options();

  std::shared_ptr<Object> add_object(Object* new_object);
  std::shared_ptr<Logpath> add_logpath(Logpath* new_logpath);

  void parse_yaml(const std::string& file_name);

  const std::string to_string() const;

private:
  void update_objects_id(const std::string& name, const std::string& type);
  void delete_object(const Object* old_object);
  void delete_logpath(const Logpath* old_logpath);

  bool deleted = false;
};

#endif  // CONFIG_H
