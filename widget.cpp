#include "widget.h"
#include "driver.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QLine>

Widget::Widget(std::vector<Driver>& drivers, QWidget* parent) :
  QWidget(parent),
  drivers(drivers)
{
  setMouseTracking(true);
}

Widget::~Widget()
{}

void Widget::add_driver(Driver* driver)
{
  selected_driver = driver;
}

Driver* Widget::get_nearest_driver(const QPoint& point)
{
  Driver* nearest = nullptr;
  double max = 25.0;

  for (Driver& driver : drivers)
  {
    const double dist = QLineF(driver.get_location(), point).length();
    if (dist < max)
    {
      max = dist;
      nearest = &driver;
    }
  }

  return nearest;
}

void Widget::mousePressEvent(QMouseEvent* event)
{
  if (selected_driver != nullptr)
  {
    selected_driver = nullptr;
  }
  else
  {
    selected_driver = get_nearest_driver(event->pos());
  }

  update();
}

void Widget::mouseDoubleClickEvent(QMouseEvent *)
{
  if (selected_driver != nullptr)
  {
    emit update_driver(*selected_driver);

    selected_driver = nullptr;
  }
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
  if (selected_driver != nullptr)
  {
    selected_driver->set_location(event->pos());

    update();
  }
}

void Widget::keyPressEvent(QKeyEvent* event)
{
  if (selected_driver != nullptr)
  {
    if (event->key() == Qt::Key_Delete)
    {
      const std::string& name = selected_driver->get_name();
      const std::string& type = selected_driver->get_type();
      const int id = selected_driver->get_id();

      drivers.erase(std::find(drivers.begin(), drivers.end(), *selected_driver));

      for (Driver& driver : drivers)
      {
        if (driver.get_name() == name && driver.get_type() == type && driver.get_id() > id)
        {
          driver.set_id(driver.get_id() - 1);
        }
      }
    }

    selected_driver = nullptr;

    update();
  }
}

void Widget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  for (const Driver& driver : drivers)
  {
    painter.setPen(&driver == selected_driver ? QPen(QBrush(Qt::black), 3) : QPen(QBrush(Qt::black), 1));

    const QPoint& location = driver.get_location();
    const std::string id = driver.get_name() + "_" + std::to_string(driver.get_id());

    const char type = driver.get_type().at(0);
    switch (type)
    {
      case 's':
      {
        painter.setBrush(QColor(Qt::blue));
        painter.drawEllipse(location.x() - 20, location.y() - 20, 40, 40);
        painter.drawText(location.x() - 20, location.y() - 25, QString::fromStdString(id));
        break;
      }
      case 'd':
      {
        painter.setBrush(QColor(Qt::red));
        painter.drawRect(location.x() - 30, location.y() - 20, 60, 40);
        painter.drawText(location.x() - 30, location.y() - 25, QString::fromStdString(id));
        break;
      }
      case 'p':
      {
        /*QPainterPath path;
        path.moveTo(points.at(0).x(), points.at(0).y());
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
