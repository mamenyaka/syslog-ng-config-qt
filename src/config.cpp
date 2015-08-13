#include "config.h"

#include <QDirIterator>

#include <yaml-cpp/yaml.h>
#include <map>

template <class T, class U>
struct Typelist
{
  typedef T Head;
  typedef U Tail;
};

class NullType {};

typedef Typelist< Source, Typelist<Destination,
  Typelist<Filter, Typelist<Template,
  Typelist<Rewrite, Typelist<Parser,
  Typelist<Options, NullType> > > > > > > ObjectTypes;

typedef Typelist< StringOption, Typelist<NumberOption,
  Typelist<ListOption, Typelist<SetOption,
  Typelist<ExternOption, NullType> > > > > OptionTypes;

template<class TList, unsigned int index>
struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0>
{
  typedef Head Result;
};

template<class Head, class Tail, unsigned int index>
struct TypeAt<Typelist<Head, Tail>, index>
{
  typedef typename TypeAt<Tail, index - 1>::Result Result;
};

template<unsigned int index>
Object* create_object(const std::string& name, const std::string& description)
{
  return new typename TypeAt<ObjectTypes, index>::Result(name, description);
}

template<unsigned int index>
Option* create_option(const std::string& name, const std::string& description)
{
  return new typename TypeAt<OptionTypes, index>::Result(name, description);
}


Config::Config(const std::string& dir_name)
{
  default_objects.reserve(60);

  QDirIterator it(QString::fromStdString(dir_name));
  while (it.hasNext())
  {
    it.next();
    if (!it.fileInfo().isFile())
    {
      continue;
    }

    parse_yaml(it.filePath().toStdString());
  }

  std::sort(default_objects.begin(), default_objects.end(),
            [](std::unique_ptr<const Object>& a, std::unique_ptr<const Object>& b)->bool {
              return a->get_name() < b->get_name();
            });

  const Options& options_global = static_cast<const Options&>(get_default_object("global", "options"));
  const Options& options_tls = static_cast<const Options&>(get_default_object("tls", "options"));
  const Options& options_value_pairs = static_cast<const Options&>(get_default_object("value-pairs", "options"));

  global_options = std::make_unique<GlobalOptions>(options_global);

  for (std::unique_ptr<const Object>& object : default_objects)
  {
    for (const std::unique_ptr<Option>& option : object->get_options())
    {
      if (option->get_name() == "tls")
      {
        static_cast<ExternOption&>(*option).set_options(options_tls);
        break;
      }

      if (option->get_name() == "value-pairs")
      {
        static_cast<ExternOption&>(*option).set_options(options_value_pairs);
        break;
      }
    }
  }
}

Config::~Config()
{
  deleted = true;
}

const std::vector< std::unique_ptr<const Object> >& Config::get_default_objects() const
{
  return default_objects;
}

const Object& Config::get_default_object(const std::string& name, const std::string& type) const
{
  auto it = std::find_if(default_objects.cbegin(), default_objects.cend(),
                         [&name, &type](const std::unique_ptr<const Object>& object)->bool {
                           return object->get_name() == name && object->get_type() == type;
                         });
  return **it;
}

std::unique_ptr<GlobalOptions>& Config::get_global_options()
{
  return global_options;
}

std::shared_ptr<Object> Config::add_object(Object* new_object)
{
  const std::string name = new_object->get_name();
  const std::string type = new_object->get_type();

  objects.emplace_back(new_object);

  update_objects_id(name, type);

  std::unique_ptr<Object>& object = objects.back();

  return std::shared_ptr<Object>(object.get(), [&](const Object* object) { delete_object(object); });
}

std::shared_ptr<Logpath> Config::add_logpath(Logpath* new_logpath)
{
  logpaths.emplace_back(new_logpath);

  std::unique_ptr<Logpath>& logpath = logpaths.back();

  return std::shared_ptr<Logpath>(logpath.get(), [&](const Logpath* logpath) { delete_logpath(logpath); });
}

void Config::parse_yaml(const std::string& file_name)
{
  const YAML::Node yaml_object = YAML::LoadFile(file_name);

  const std::string name = yaml_object["name"].as<std::string>();
  const std::string type = yaml_object["type"].as<std::string>();
  const std::string description = yaml_object["description"].as<std::string>();

  std::map<std::string, void*> object_map {
    {"source", create_object<0>(name, description)},
    {"destination", create_object<1>(name, description)},
    {"filter", create_object<2>(name, description)},
    {"template", create_object<3>(name, description)},
    {"rewrite", create_object<4>(name, description)},
    {"parser", create_object<5>(name, description)},
    {"options", create_object<6>(name, description)}
  };

  Object* object = static_cast<Object*>(object_map[type]);

  if (yaml_object["include"])
  {
    object->set_include(yaml_object["include"].as<std::string>());
  }

  const YAML::Node& options = yaml_object["options"];
  for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
  {
    YAML::const_iterator tmp = option_it->begin();
    const YAML::Node& yaml_option = tmp->second;

    const std::string name = tmp->first.as<std::string>();
    const std::string type = yaml_option["type"].as<std::string>();
    const std::string description = yaml_option["description"].as<std::string>();

    std::map<std::string, void*> option_map {
      {"string", create_option<0>(name, description)},
      {"number", create_option<1>(name, description)},
      {"list", create_option<2>(name, description)},
      {"set", create_option<3>(name, description)},
      {"tls", create_option<4>(name, description)},
      {"value-pairs", create_option<4>(name, description)}
    };

    Option* option = static_cast<Option*>(option_map[type]);

    const YAML::Node& values = yaml_option["values"];
    for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
    {
      const std::string value = value_it->as<std::string>();

      if (typeid(*option) == typeid(ListOption))
      {
        static_cast<ListOption*>(option)->add_value(value);
      }
      else if (typeid(*option) == typeid(SetOption))
      {
        static_cast<SetOption*>(option)->add_value(value);
      }
    }

    if (yaml_option["default"])
    {
      option->set_default(yaml_option["default"].as<std::string>());
    }

    if (yaml_option["required"])
    {
      option->set_required(yaml_option["required"].as<bool>());
    }

    object->add_option(option);
  }

  default_objects.emplace_back(object);
}

const std::string Config::to_string() const
{
  std::string config;

  config += global_options->to_string();

  for (const std::unique_ptr<Object>& object : objects)
  {
    config += object->to_string();
  }

  for (const std::unique_ptr<Logpath>& logpath : logpaths)
  {
    config += logpath->to_string();
  }

  return config;
}

void Config::update_objects_id(const std::string& name, const std::string& type)
{
  int id = 0;
  for (std::unique_ptr<Object>& object : objects)
  {
    if (object->get_name() == name && object->get_type() == type)
    {
      object->set_id(id++);
    }
  }
}

void Config::delete_object(const Object* old_object)
{
  if (deleted)
  {
    return;
  }

  const std::string name = old_object->get_name();
  const std::string type = old_object->get_type();

  objects.remove_if([old_object](const std::unique_ptr<Object>& object)->bool { return object.get() == old_object; });

  update_objects_id(name, type);
}

void Config::delete_logpath(const Logpath* old_logpath)
{
  if (deleted)
  {
    return;
  }

  logpaths.remove_if([old_logpath](const std::unique_ptr<Logpath>& logpath)->bool { return logpath.get() == old_logpath; });
}
