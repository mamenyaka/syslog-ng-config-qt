#ifndef SHAPE_H
#define SHAPE_H

#include <QPoint>

#include <string>

class Shape
{
  std::string id;
  QPoint location;

public:
  Shape(const std::string& id) :
    id(id)
  {}

  const std::string& get_id() const
  {
    return id;
  }
  char get_type() const
  {
    return id[0];
  }
  QPoint& get_location()
  {
    return location;
  }
};

#endif  // SHAPE_H
