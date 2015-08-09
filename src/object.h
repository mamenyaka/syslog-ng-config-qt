#ifndef OBJECT_H
#define OBJECT_H

#include "option.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

class QPainter;

class Object
{
protected:
  std::string name;
  std::string description;
  std::vector< std::unique_ptr<Option> > options;
  std::string include;
  int id = -1;

public:
  Object(const std::string& name,
         const std::string& description);
  Object(const Object& other);

  virtual Object* clone() const = 0;

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
  virtual const std::string get_separator() const;
  virtual const std::string to_string() const;
};

template<class Derived>
class ObjectBase : public Object
{
public:
  ObjectBase(const std::string& name,
             const std::string& description);

  Object* clone() const;
};

class Source : public ObjectBase<Source>
{
public:
  Source(const std::string& name,
         const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Destination : public ObjectBase<Destination>
{
public:
  Destination(const std::string& name,
              const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Filter : public ObjectBase<Filter>
{
public:
  Filter(const std::string& name,
         const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Template : public ObjectBase<Template>
{
public:
  Template(const std::string& name,
           const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Rewrite : public ObjectBase<Rewrite>
{
public:
  Rewrite(const std::string& name,
          const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
  const std::string get_separator() const;
};

class Parser : public ObjectBase<Parser>
{
public:
  Parser(const std::string& name,
         const std::string& description);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
};

class Options : public ObjectBase<Options>
{
  std::string separator;

public:
  Options(const std::string& name,
          const std::string& description);

  void set_separator(const std::string& separator);

  void draw(QPainter *, int , int ) const {}

  const std::string get_type() const;
  const std::string to_string() const;

  const std::string& get_include() const = delete;
  int get_id() const = delete;

  void add_option(Option* option) = delete;
  void set_include(const std::string& include) = delete;
  void set_id(int id) = delete;
};

class Logpath
{
  Options options;
  std::list< std::shared_ptr<const Object> > objects;

public:
  Logpath(const Options& options);

  Options& get_options();

  void add_object(const std::shared_ptr<Object>& object);
  void remove_object(const std::shared_ptr<const Object>& object);

  const std::string to_string() const;
};

class GlobalOptions
{
  Options options;

public:
  GlobalOptions(const Options& options);

  Options& get_options();

  const std::string to_string() const;

private:
  bool has_changed() const;
};

#endif  // OBJECT_H
