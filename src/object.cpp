#include "object.h"

#include <QPainter>

#include <cmath>

Object::Object(const std::string& name,
               const std::string& description) :
  name(name),
  description(description)
{
  options.reserve(20);
}

Object::Object(const Object& other) :
 name(other.name),
 description(other.description)
{
  for (const std::unique_ptr<Option>& option : other.options)
  {
    options.emplace_back(option->clone());
  }
}

const std::string& Object::get_name() const
{
  return name;
}

const std::string& Object::get_description() const
{
  return description;
}

std::vector< std::unique_ptr<Option> >& Object::get_options()
{
  return options;
}

const std::vector< std::unique_ptr<Option> >& Object::get_options() const
{
  return options;
}

void Object::add_option(Option* option)
{
  options.emplace_back(option);
}

const std::string Object::get_separator() const
{
  return "";
}

const std::string Object::to_string() const
{
  std::string config;

  config += name + "(";

  for (const std::unique_ptr<Option>& option : options)
  {
    if (name == option->get_name())
    {
      config += option->get_current_value() + get_separator();
      continue;
    }

    if (!option->has_changed())
    {
      continue;
    }

    config += "\n        " + option->to_string() + get_separator();
  }

  if (config.back() == ',')
  {
    config.pop_back();
  }

  config += ");";

  return config;
}


template<class Derived>
ObjectBase<Derived>::ObjectBase(const std::string& name,
                                const std::string& description) :
  Object(name, description)
{}

template<class Derived>
Object* ObjectBase<Derived>::clone() const
{
  return new Derived(static_cast<const Derived&>(*this));
}

Source::Source(const std::string& name,
               const std::string& description) :
  ObjectBase<Source>(name, description)
{}

void Source::draw(QPainter* painter, int width, int height) const
{
  painter->setBrush(QColor(255, 128, 128, 192));
  painter->drawEllipse(1, 1, width - 2, height - 2);
}

const std::string Source::get_type() const
{
  return "source";
}


Destination::Destination(const std::string& name,
                         const std::string& description) :
  ObjectBase<Destination>(name, description)
{}

void Destination::draw(QPainter* painter, int width, int height) const
{
  painter->setBrush(QColor(128, 128, 225, 192));
  painter->drawRect(1, 1, width - 2, height - 2);
}

const std::string Destination::get_type() const
{
  return "destination";
}


Filter::Filter(const std::string& name,
               const std::string& description) :
  ObjectBase<Filter>(name, description)
{}

void Filter::set_invert(bool invert)
{
  this->invert = invert;
}

void Filter::set_next(const std::string& next)
{
  this->next = next;
}

void Filter::draw(QPainter* painter, int width, int height) const
{
  QPainterPath path;
  path.moveTo(width/2.0, 1);
  path.lineTo(width - 1, height/2.0);
  path.lineTo(width/2.0, height - 1);
  path.lineTo(1, height/2.0);
  path.closeSubpath();

  painter->setBrush(QColor(128, 255, 128, 192));
  painter->drawPath(path);
  painter->fillPath(path, painter->brush());
}

const std::string Filter::get_type() const
{
  return "filter";
}

const std::string Filter::to_string() const
{
  std::string config;

  config += (invert ? "not " : "");
  config += Object::to_string();
  config.pop_back();
  config += " " + next;

  return config;
}


Template::Template(const std::string& name,
                   const std::string& description) :
  ObjectBase<Template>(name, description)
{}

void Template::draw(QPainter* painter, int width, int height) const
{
  painter->setPen(QColor(255, 128, 255, 192));
  painter->drawEllipse(1, 1, width - 2, height - 2);
}

const std::string Template::get_type() const
{
  return "template";
}


Rewrite::Rewrite(const std::string& name,
                 const std::string& description) :
  ObjectBase<Rewrite>(name, description)
{}

void Rewrite::draw(QPainter* painter, int width, int height) const
{
  const double pi = std::acos(-1);
  double h = (1 - std::cos(pi/6.0))*(height - 2)/2;

  QPainterPath path;
  path.moveTo(width/2.0, 1);
  path.lineTo(width - 1 - h, 1 + (height - 2)*1/4.0);
  path.lineTo(width - 1 - h, 1 + (height - 2)*3/4.0);
  path.lineTo(width/2.0, height - 1);
  path.lineTo(1 + h, 1 + (height - 2)*3/4.0);
  path.lineTo(1 + h, 1 + (height - 2)*1/4.0);
  path.closeSubpath();

  painter->setBrush(QColor(128, 255, 255, 192));
  painter->drawPath(path);
  painter->fillPath(path, painter->brush());
}

const std::string Rewrite::get_type() const
{
  return "rewrite";
}

const std::string Rewrite::get_separator() const
{
  return ",";
}


Parser::Parser(const std::string& name,
               const std::string& description) :
  ObjectBase<Parser>(name, description)
{}

void Parser::draw(QPainter* painter, int width, int height) const
{
  painter->setBrush(QColor(255, 16, 32));
  painter->drawEllipse(1 + width*1/4.0, 1, (width - 2)/2.0, (height - 2)/2.0);
  painter->setBrush(QColor(0, 128, 32));
  painter->drawEllipse(1 + width*1/4.0, height/2.0, (width - 2)/2.0, (height - 2)/2.0);
  painter->setBrush(QColor(64, 128, 255));
  painter->drawEllipse(1, 1 + (height - 2)*1/4.0, (width - 2)/2.0, (height - 2)/2.0);
  painter->setBrush(QColor(255, 192, 16));
  painter->drawEllipse(width/2.0, 1 + (height - 2)*1/4.0, (width - 2)/2.0, (height - 2)/2.0);
}

const std::string Parser::get_type() const
{
  return "parser";
}


Options::Options(const std::string& name,
                 const std::string& description) :
  ObjectBase<Options>(name, description)
{}

void Options::set_separator(const std::string& separator)
{
  this->separator = separator;
}

const std::string Options::get_type() const
{
  return "options";
}

const std::string Options::to_string() const
{
  std::string config;

  for (const std::unique_ptr<Option>& option : options)
  {
    if (!option->has_changed())
    {
      continue;
    }

    config += "\n    " + option->to_string() + separator;
  }

  return config;
}


GlobalOptions::GlobalOptions(const Options& options) :
  options(options)
{
  this->options.set_separator(";");
}

Options& GlobalOptions::get_options()
{
  return options;
}

const std::string GlobalOptions::to_string() const
{
  std::string config;

  if (!has_changed())
  {
    return config;
  }

  config += "options {";

  config += options.to_string();

  config += "\n};\n\n";

  return config;
}

bool GlobalOptions::has_changed() const
{
  for (const std::unique_ptr<Option>& option : options.get_options())
  {
    if (option->has_changed())
    {
      return true;
    }
  }

  return false;
}


ObjectStatement::ObjectStatement(const std::string& name) :
  name(name)
{}

const std::string& ObjectStatement::get_name() const
{
  return name;
}

const std::string& ObjectStatement::get_type() const
{
  return type;
}

const std::list< std::shared_ptr<const Object> >& ObjectStatement::get_objects() const
{
  return objects;
}

void ObjectStatement::add_object(const std::shared_ptr<const Object>& object, const int position)
{
  if (objects.empty())
  {
    type = object->get_type();
  }

  auto it = objects.begin();
  std::advance(it, position);
  objects.insert(it, object);
}

void ObjectStatement::remove_object(const std::shared_ptr<const Object>& object)
{
  objects.remove(object);

  if (objects.empty())
  {
    type.clear();
  }
}

const std::string ObjectStatement::to_string() const
{
  std::string config;

  if (objects.empty())
  {
    return config;
  }

  config += type + " " + name + " {";

  for (const std::shared_ptr<const Object>& object : objects)
  {
    config += "\n    " + object->to_string();
  }

  if (type == "filter")
  {
    config.pop_back();
    config += ";";
  }

  config += "\n};\n\n";

  return config;
}


LogStatement::LogStatement(const Options& options) :
  options(options)
{
  this->options.set_separator(";");
}

Options& LogStatement::get_options()
{
  return options;
}

const std::list< std::shared_ptr<const ObjectStatement> >& LogStatement::get_object_statements() const
{
  return object_statements;
}

void LogStatement::add_object_statement(const std::shared_ptr<const ObjectStatement>& object_statement, const int position)
{
  auto it = object_statements.begin();
  std::advance(it, position);
  object_statements.insert(it, object_statement);
}

void LogStatement::remove_object_statement(const std::shared_ptr< const ObjectStatement >& object_statement)
{
  object_statements.remove(object_statement);
}

const std::string LogStatement::to_string() const
{
  std::string config;

  if (object_statements.empty())
  {
    return config;
  }

  config += "log {";

  for (const std::shared_ptr<const ObjectStatement>& object_statement : object_statements)
  {
    config += "\n    " + object_statement->get_type() + "(" + object_statement->get_name() + ");";
  }

  config += options.to_string();

  config += "\n};\n\n";

  return config;
}
