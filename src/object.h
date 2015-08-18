#ifndef OBJECT_H
#define OBJECT_H

#include "option.h"

#include <list>

class QPainter;

enum class ObjectType { SOURCE, DESTINATION, FILTER, TEMPLATE, REWRITE, PARSER, OPTIONS };

class Object
{
protected:
  std::string name;
  std::string description;
  std::vector< std::unique_ptr<Option> > options;

public:
  Object(const std::string& name,
         const std::string& description);
  Object(const Object& other);
  virtual ~Object() {}

  virtual Object* clone() const = 0;

  const std::string& get_name() const;
  const std::string& get_description() const;
  std::vector< std::unique_ptr<Option> >& get_options();
  const std::vector< std::unique_ptr<Option> >& get_options() const;

  void add_option(Option* option);

  virtual void draw(QPainter* painter, int width, int height) const = 0;

  virtual const std::string get_type() const = 0;
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
  bool invert = false;
  std::string next;

public:
  Filter(const std::string& name,
         const std::string& description);

  void set_invert(bool invert);
  void set_next(const std::string& next);

  void draw(QPainter* painter, int width, int height) const;

  const std::string get_type() const;
  const std::string to_string() const;
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
};

class GlobalOptions
{
  Options options;

public:
  explicit GlobalOptions(const Options& options);

  Options& get_options();

  const std::string to_string() const;

private:
  bool has_changed() const;
};

class ObjectStatement
{
  std::string name;
  std::string type;
  std::list< std::shared_ptr<const Object> > objects;

public:
  explicit ObjectStatement(const std::string& name);

  const std::string& get_name() const;
  const std::string& get_type() const;
  const std::list< std::shared_ptr<const Object> >& get_objects() const;

  void add_object(const std::shared_ptr<const Object>& object, const int position);
  void remove_object(const std::shared_ptr<const Object>& object);

  const std::string to_string() const;
};

class LogStatement
{
  Options options;
  std::list< std::shared_ptr<const ObjectStatement> > object_statements;

public:
  explicit LogStatement(const Options& options);

  Options& get_options();
  const std::list< std::shared_ptr<const ObjectStatement> >& get_object_statements() const;

  void add_object_statement(const std::shared_ptr<const ObjectStatement>& object_statement, const int position);
  void remove_object_statement(const std::shared_ptr<const ObjectStatement>& object_statement);

  const std::string to_string() const;
};

#endif  // OBJECT_H
