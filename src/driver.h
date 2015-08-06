#ifndef DRIVER_H
#define DRIVER_H

#include "option.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

class QPainter;

class Driver
{
protected:
  std::string name;
  std::string description;
  std::vector< std::unique_ptr<Option> > options;
  std::string include;
  int id = -1;

public:
  Driver(const std::string& name,
         const std::string& description);
  Driver(const Driver& other);

  virtual Driver* clone() const = 0;

  const std::string& get_name() const;
  const std::string& get_description() const;
  std::vector< std::unique_ptr<Option> >& get_options();
  const std::vector< std::unique_ptr<Option> >& get_options() const;
  const std::string& get_include() const;
  int get_id() const;

  void add_option(Option* option);
  void set_include(const std::string& include);
  void set_id(int id);

  void set_previous_values();
  void restore_previous_values();
  void restore_default_values();

  virtual void draw(QPainter* painter, int width, int height) const = 0;

  virtual const std::string get_type() const = 0;
  virtual const std::string get_id_name() const;
  virtual const std::string to_string() const;
};

template<class Derived>
class DriverBase : public Driver
{
public:
  DriverBase(const std::string& name,
             const std::string& description);

  Driver* clone() const;
};

class Source : public DriverBase<Source>
{
public:
  Source(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Destination : public DriverBase<Destination>
{
public:
  Destination(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Filter : public DriverBase<Filter>
{
public:
  Filter(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Template : public DriverBase<Template>
{
public:
  Template(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Rewrite : public DriverBase<Rewrite>
{
public:
  Rewrite(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;

  const std::string to_string() const;
};

class Parser : public DriverBase<Parser>
{
public:
  Parser(const std::string& name, const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Options : public DriverBase<Options>
{
public:
  Options(const std::string& name, const std::string& description);

  void draw(QPainter *, int , int ) const {}

  const std::string to_string() const;

  const std::string get_type() const;

  const std::string& get_include() const = delete;
  int get_id() const = delete;

  void add_option(Option* option) = delete;
  void set_include(const std::string& include) = delete;
  void set_id(int id) = delete;
};

class Log
{
  Options options;
  std::list< std::shared_ptr<const Driver> > drivers;

public:
  Log(const Options& options);

  Options& get_options();

  void add_driver(const std::shared_ptr<Driver>& driver);
  void remove_driver(const std::shared_ptr<const Driver>& driver);

  bool has_changed() const;
  const std::string to_string() const;
};

class GlobalOptions
{
  Options options;

public:
  GlobalOptions(const Options& options);

  Options& get_options();

  bool has_changed() const;
  const std::string to_string() const;
};

#endif  // DRIVER_H
