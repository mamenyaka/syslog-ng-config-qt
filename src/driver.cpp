#include "driver.h"

#include <QPainter>

#include <cmath>

Driver::Driver(const std::string& name,
               const std::string& description) :
  name(name),
  description(description)
{}

Driver::Driver(const Driver& other) :
 name(other.name),
 description(other.description),
 include(other.include),
 id(other.id)
{
  for (const std::unique_ptr<Option>& option : other.options)
  {
    options.emplace_back(option->clone());
  }
}

const std::string& Driver::get_name() const
{
  return name;
}

const std::string& Driver::get_description() const
{
  return description;
}

std::vector< std::unique_ptr<Option> >& Driver::get_options()
{
  return options;
}

const std::vector< std::unique_ptr<Option> >& Driver::get_options() const
{
  return options;
}

const std::string& Driver::get_include() const
{
  return include;
}

int Driver::get_id() const
{
  return id;
}

void Driver::add_option(Option* option)
{
  options.emplace_back(option);
}

void Driver::set_include(const std::string& include)
{
  this->include = include;
}

void Driver::set_id(int id)
{
  this->id = id;
}

void Driver::set_previous_values()
{
  for (std::unique_ptr<Option>& option : options)
  {
    option->set_previous();
  }
}

void Driver::restore_previous_values()
{
  for (std::unique_ptr<Option>& option : options)
  {
    option->restore_previous();
  }
}

void Driver::restore_default_values()
{
  for (std::unique_ptr<Option>& option : options)
  {
    option->restore_default();
  }
}

const std::string Driver::get_id_name() const
{
  return std::string(1, get_type().at(0)) + "_" + name + std::to_string(id);
}

const std::string Driver::to_string() const
{
  std::string config;

  if (!include.empty())
  {
    config += "@include \"" + include + "\"\n\n";
  }

  config += get_type() + " " + get_id_name() + " {";
  config += "\n    " + name + "(";

  for (const std::unique_ptr<Option>& option : options)
  {
    if (name == option->get_name())
    {
      config += option->get_current_value();
      continue;
    }

    if (!option->has_changed())
    {
      continue;
    }

    config += "\n        " + option->to_string();
  }

  config += "\n    );";
  config += "\n};\n\n";

  return config;
}


template<class Derived>
DriverBase<Derived>::DriverBase(const std::string& name,
                       const std::string& description) :
  Driver(name, description)
{}

template<class Derived>
Driver* DriverBase<Derived>::clone() const
{
  return new Derived(static_cast<const Derived&>(*this));
}


Source::Source(const std::string& name, const std::string& description) :
  DriverBase(name, description)
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


Destination::Destination(const std::string& name, const std::string& description) :
  DriverBase(name, description)
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


Filter::Filter(const std::string& name, const std::string& description) :
  DriverBase(name, description)
{}

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


Template::Template(const std::string& name, const std::string& description) :
  DriverBase(name, description)
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


Rewrite::Rewrite(const std::string& name, const std::string& description) :
  DriverBase(name, description)
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

const std::string Rewrite::to_string() const
{
  std::string config;

  if (!include.empty())
  {
    config += "@include \"" + include + "\"\n\n";
  }

  config += get_type() + " " + get_id_name() + " {";
  config += "\n    " + name + "(";

  for (const std::unique_ptr<Option>& option : options)
  {
    if (name == option->get_name())
    {
      config += option->get_current_value() + ", ";
      continue;
    }

    if (config.back() == ' ')
    {
      config.pop_back();
      config.pop_back();
    }

    if (!option->has_changed())
    {
      continue;
    }

    config += ",\n        " + option->to_string();
  }

  config += "\n    );";
  config += "\n};\n\n";

  return config;
}


Parser::Parser(const std::string& name, const std::string& description) :
  DriverBase(name, description)
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


Options::Options(const std::string& name, const std::string& description) :
  DriverBase(name, description)
{}

const std::string Options::to_string() const
{
  std::string config;

  for (const std::unique_ptr<Option>& option : options)
  {
    if (!option->has_changed())
    {
      continue;
    }

    config += "\n    " + option->to_string();
  }

  return config;
}

const std::string Options::get_type() const
{
  return "options";
}


Log::Log(const Options& options) :
  options(options)
{}

Options& Log::get_options()
{
  return options;
}

void Log::add_driver(const std::shared_ptr<Driver>& driver)
{
  drivers.push_back(driver);
}

void Log::remove_driver(const std::shared_ptr<const Driver>& driver)
{
  drivers.remove(driver);
}

bool Log::has_changed() const
{
  return !drivers.empty();
}

const std::string Log::to_string() const
{
  std::string config;

  if (has_changed())
  {
    config += "log {";

    for (const std::shared_ptr<const Driver>& driver : drivers)
    {
      config += "\n    " + driver->get_type() + "(" + driver->get_id_name() + ");";
    }

    const std::unique_ptr<Option>& flags = options.get_options().front();
    if (flags->has_changed())
    {
      config += "\n    " + flags->to_string() + ";";
    }

    config += "\n};\n\n";
  }

  return config;
}


GlobalOptions::GlobalOptions(const Options& options) :
  options(options)
{}

Options& GlobalOptions::get_options()
{
  return options;
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

const std::string GlobalOptions::to_string() const
{
  std::string config;

  if (has_changed())
  {
    config += "options {";

    for (const std::unique_ptr<Option>& option : options.get_options())
    {
      if (!option->has_changed())
      {
        continue;
      }

      config += "\n    " + option->to_string() + ";";
    }

    config += "\n};\n\n";
  }

  return config;
}
