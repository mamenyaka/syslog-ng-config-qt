#ifndef WIDGET_H
#define WIDGET_H

#include "shape.h"

#include <QWidget>

#include <vector>

class Widget : public QWidget
{
  Q_OBJECT

  std::vector<Shape> shapes;
  Shape* shape_to_move = nullptr;

public:
  explicit Widget(QWidget* parent = 0);
  ~Widget();

public slots:
  void add_shape(const std::string& id);

signals:
  void update_driver(const std::string& id);
  void delete_driver(const std::string& id);
  void driver_added();

protected:
  void paintEvent(QPaintEvent *);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

private:
  Shape* get_nearest(const QPoint& point);
  std::vector<Shape>::iterator get_shape_iterator(const std::string& id);
};

#endif  // WIDGET_H
