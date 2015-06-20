#include "widget.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QLine>

Widget::Widget(QWidget* parent) :
  QWidget(parent)
{
  setMouseTracking(true);
}

Widget::~Widget()
{}

void Widget::add_shape(const std::string& id)
{
  Shape shape(id);
  shapes.push_back(std::move(shape));

  shape_to_move = &shapes.back();
}

Shape* Widget::get_nearest(const QPoint& point)
{
  Shape* nearest = nullptr;
  double max = 25.0;
  for (Shape& shape : shapes)
  {
    double dist = QLineF(shape.get_location(), point).length();
    if (dist < max)
    {
      max = dist;
      nearest = &shape;
    }
  }

  return nearest;
}

std::vector<Shape>::iterator Widget::get_shape_iterator(const std::string& id)
{
  for (std::vector<Shape>::iterator it = shapes.begin(); it != shapes.end(); ++it)
  {
    const Shape& shape = *it;
    if (shape.get_id() == id)
    {
      return it;
    }
  }

  return shapes.end();
}

void Widget::mousePressEvent(QMouseEvent* event)
{
  if (shape_to_move != nullptr)
  {
    shape_to_move = nullptr;
    emit driver_added();

    return;
  }

  shape_to_move = get_nearest(event->pos());

  if (event->button() == Qt::RightButton && shape_to_move != nullptr)
  {
    const std::string& id = shape_to_move->get_id();

    emit delete_driver(id);
    shapes.erase(get_shape_iterator(id));

    shape_to_move = nullptr;

    update();
  }
}

void Widget::mouseDoubleClickEvent(QMouseEvent* event)
{
  Shape* nearest = get_nearest(event->pos());

  if (nearest != nullptr)
  {
    emit update_driver(nearest->get_id());
  }

  shape_to_move = nullptr;
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
  if (shape_to_move != nullptr)
  {
    shape_to_move->get_location() = event->pos();

    update();
  }
}

void Widget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  for (Shape& shape : shapes)
  {
    painter.setPen(QPen(Qt::black));

    QPoint& location = shape.get_location();

    switch (shape.get_type())
    {
      case 's':
      {
        painter.setBrush(QColor(Qt::blue));
        painter.drawEllipse(location.x() - 20, location.y() - 20, 40, 40);
        painter.drawText(location.x() - 20, location.y() - 25, QString::fromStdString(shape.get_id()));
        break;
      }
      case 'd':
      {
        painter.setBrush(QColor(Qt::red));
        painter.drawRect(location.x() - 30, location.y() - 20, 60, 40);
        painter.drawText(location.x() - 30, location.y() - 25, QString::fromStdString(shape.get_id()));
        break;
      }
      case 'p':
      {
        QPainterPath path;
        /*path.moveTo(points.at(0).x(), points.at(0).y());
        path.lineTo(points.at(1).x(), points.at(1).y());
        path.lineTo(points.at(2).x(), points.at(2).y());
        path.lineTo(points.at(0).x(), points.at(0).y());

        painter.fillPath(path, painter.brush());*/
        break;
      }
    }
  }

  painter.end();
}
