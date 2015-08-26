/*
 * Copyright (C) 2015 Andras Mamenyak
 *
 * This file is part of syslog-ng-config-qt.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <QDirIterator>

#include <yaml-cpp/yaml.h>
#include <map>

// Typelist and TypeAt from the Loki library, described in Alexandrescu's Modern C++ Design book
template <class T, class U>
struct Typelist
{
  typedef T Head;
  typedef U Tail;
};

class NullType {};

typedef Typelist<Source,
  Typelist<Destination,
  Typelist<Filter,
  Typelist<Template,
  Typelist<Rewrite,
  Typelist<Parser,
  Typelist<Options, NullType> > > > > > > ObjectTypes;

typedef Typelist<StringOption,
  Typelist<NumberOption,
  Typelist<ListOption,
  Typelist<SetOption,
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

template<ObjectType type>
Object* create_object(const std::string& name, const std::string& description)
{
  return new typename TypeAt<ObjectTypes, (int) type>::Result(name, description);
}

template<OptionType type>
Option* create_option(const std::string& name, const std::string& description)
{
  return new typename TypeAt<OptionTypes, (int) type>::Result(name, description);
}

std::map<std::string, Object*(*)(const std::string&, const std::string&)> create_object_map {
  {"source", create_object<ObjectType::SOURCE>},
  {"destination", create_object<ObjectType::DESTINATION>},
  {"filter", create_object<ObjectType::FILTER>},
  {"template", create_object<ObjectType::TEMPLATE>},
  {"rewrite", create_object<ObjectType::REWRITE>},
  {"parser", create_object<ObjectType::PARSER>},
  {"options", create_object<ObjectType::OPTIONS>}
};

std::map<std::string, Option*(*)(const std::string&, const std::string&)> create_option_map {
  {"string", create_option<OptionType::STRING>},
  {"number", create_option<OptionType::NUMBER>},
  {"list", create_option<OptionType::LIST>},
  {"set", create_option<OptionType::SET>},
  {"tls", create_option<OptionType::OPTIONS>},
  {"value-pairs", create_option<OptionType::OPTIONS>}
};


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
  global_options = std::make_unique<GlobalOptions>(options_global);

  // some objects have tls or value-pairs options, which are creted from separate yaml files and need to be set after
  for (std::unique_ptr<const Object>& object : default_objects)
  {
    for (const std::unique_ptr<Option>& option : object->get_options())
    {
      ExternOption* extern_option = dynamic_cast<ExternOption*>(option.get());
      if (extern_option)
      {
        const Options& options = static_cast<const Options&>(get_default_object(extern_option->get_type(), "options"));
        extern_option->set_options(options);
      }
    }
  }
}

Config::~Config()
{
  // if not implemented, the program segfaults at exit when the shared_ptr's deleters are called,
  // accessing resources from the already deleted Config object
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

Options& Config::get_global_options()
{
  return global_options->get_options();
}

const std::list< std::unique_ptr<ObjectStatement> >& Config::get_object_statements() const
{
  return object_statements;
}

const std::list< std::unique_ptr<LogStatement> >& Config::get_log_statements() const
{
  return log_statements;
}

std::shared_ptr<ObjectStatement> Config::add_object_statement(ObjectStatement* new_object_statement)
{
  object_statements.emplace_back(new_object_statement);

  std::unique_ptr<ObjectStatement>& object_statement = object_statements.back();

  return std::shared_ptr<ObjectStatement>(object_statement.get(),
                                          [&](const ObjectStatement* object_statement) {
                                            delete_object_statement(object_statement);
                                          });
}

std::shared_ptr<LogStatement> Config::add_log_statement(LogStatement* new_log_statement)
{
  log_statements.emplace_back(new_log_statement);

  std::unique_ptr<LogStatement>& log_statement = log_statements.back();

  return std::shared_ptr<LogStatement>(log_statement.get(),
                                       [&](const LogStatement* log_statement) {
                                         delete_log_statement(log_statement);
                                       });
}

void Config::delete_object_statement(const ObjectStatement* old_object_statement)
{
  if (deleted)
  {
    return;
  }

  object_statements.remove_if([old_object_statement](const std::unique_ptr<ObjectStatement>& object_statement)->bool {
    return object_statement.get() == old_object_statement;
  });
}

void Config::delete_log_statement(const LogStatement* old_log_statement)
{
  if (deleted)
  {
    return;
  }

  log_statements.remove_if([old_log_statement](const std::unique_ptr<LogStatement>& log_statement)->bool {
    return log_statement.get() == old_log_statement;
  });
}

void Config::parse_yaml(const std::string& file_name)
{
  const YAML::Node yaml_object = YAML::LoadFile(file_name);

  const std::string name = yaml_object["name"].as<std::string>();
  const std::string type = yaml_object["type"].as<std::string>();
  const std::string description = yaml_object["description"].as<std::string>();

  Object* object = static_cast<Object*>(create_object_map[type](name, description));

  const YAML::Node& options = yaml_object["options"];
  for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
  {
    YAML::const_iterator tmp = option_it->begin();
    const YAML::Node& yaml_option = tmp->second;

    const std::string name = tmp->first.as<std::string>();
    const std::string type = yaml_option["type"].as<std::string>();
    const std::string description = yaml_option["description"].as<std::string>();

    Option* option = static_cast<Option*>(create_option_map[type](name, description));

    const YAML::Node& values = yaml_option["values"];
    for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
    {
      const std::string value = value_it->as<std::string>();
      dynamic_cast<SelectOption*>(option)->add_value(value);  // not static_cast, because multiple inheritance
    }

    if (yaml_option["default"])
    {
      option->set_default(yaml_option["default"].as<std::string>());
    }

    if (yaml_option["required"])
    {
      option->set_required(yaml_option["required"].as<bool>());
    }

    ExternOption* extern_option = dynamic_cast<ExternOption*>(option);
    if (extern_option)
    {
      extern_option->set_type(type);
    }

    object->add_option(option);
  }

  default_objects.emplace_back(object);
}

const std::string Config::to_string() const
{
  std::string config;

  config += "@version: 3.7\n";
  config += "@include \"scl.conf\"\n\n";

  config += global_options->to_string();

  for (const std::unique_ptr<ObjectStatement>& object_statement : object_statements)
  {
    config += object_statement->to_string();
  }

  for (const std::unique_ptr<LogStatement>& log_statement : log_statements)
  {
    config += log_statement->to_string();
  }

  return config;
}
