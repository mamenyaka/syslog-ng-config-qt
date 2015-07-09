#include "widget.h"
#include "config.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QLine>

Widget::Widget(Config& config, QWidget* parent) :
  QWidget(parent),
  config(config)
{
  setMouseTracking(true);
}

Widget::~Widget()
{}

void Widget::clear()
{
  emit update_statusbar("");

  selected_driver = nullptr;
  selected_log = nullptr;
  log_update = false;
}

void Widget::add_driver(Driver* driver)
{
  clear();
  selected_driver = driver;
  update();
}

void Widget::add_log(Log* log)
{
  clear();
  selected_log = log;
  update();
}

Driver* Widget::get_nearest_driver(const QPoint& point)
{
  Driver* nearest = nullptr;
  double max = max_dist;

  for (Driver& driver : config.get_drivers())
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

Log* Widget::get_nearest_log(const QPoint& point)
{
  Log* nearest = nullptr;
  double max = max_dist;

  for (Log& log : config.get_logs())
  {
    const double dist = QLineF(log.get_location(), point).length();
    if (dist < max)
    {
      max = dist;
      nearest = &log;
    }
  }

  return nearest;
}

void Widget::select_nearest(const QPoint& point)
{
  selected_driver = get_nearest_driver(point);
  selected_log = get_nearest_log(point);

  if (selected_driver != nullptr && selected_log != nullptr)
  {
    const double driver_dist = QLineF(selected_driver->get_location(), point).length();
    const double log_dist = QLineF(selected_log->get_location(), point).length();

    if (driver_dist < log_dist)
      selected_log = nullptr;
    else
      selected_driver = nullptr;
  }
}

void Widget::mousePressEvent(QMouseEvent* event)
{
  if (log_update)
  {
    Driver* const driver = get_nearest_driver(event->pos());

    if (driver != nullptr)
    {
      selected_log->has_driver(driver) ? selected_log->remove_driver(driver) : selected_log->add_driver(driver);

      update();
    }
  }
  else if (selected_driver != nullptr)
  {
    clear();
  }
  else if (selected_log != nullptr)
  {
    clear();
  }
  else
  {
    select_nearest(event->pos());
  }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
  if (log_update)
  {
    return;
  }

  selected_driver = nullptr;
  selected_log = nullptr;
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
  if (log_update)
  {
    return;
  }

  if (rect().contains(event->pos()))
  {
    if (selected_driver != nullptr)
    {
      selected_driver->update_location(event->pos());
      update();
    }
    else if (selected_log != nullptr)
    {
      selected_log->update_location(event->pos());
      update();
    }
  }
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (!log_update)
  {
    select_nearest(event->pos());
  }

  if (selected_driver != nullptr)
  {
    emit update_driver(*selected_driver);
    selected_driver = nullptr;
  }
  else if (selected_log != nullptr)
  {
    if ((log_update = !log_update))
    {
      emit update_statusbar("Click to select/unselect drivers, double click when finished");
    }
    else
    {
      emit update_statusbar("");
      selected_log = nullptr;
    }
  }
}

void Widget::keyPressEvent(QKeyEvent* event)
{
  if (log_update)
  {
    return;
  }

  if (event->key() == Qt::Key_Delete)
  {
    if (selected_driver != nullptr)
    {
      config.delete_driver(selected_driver);
      emit update_statusbar("");
      update();
    }
    else if (selected_log != nullptr)
    {
      config.delete_log(selected_log);
      emit update_statusbar("");
      update();
    }
  }
}

void Widget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(QPen(Qt::black));

  for (const Driver& driver : config.get_drivers())
  {
    const QPoint& location = driver.get_location();
    const std::string id = driver.get_id();

    switch (driver.get_type().at(0))
    {
      case 's':
      {
        painter.setBrush(QColor(255, 128, 128, 192));
        painter.drawEllipse(location.x() - 20, location.y() - 20, 40, 40);
        painter.drawText(location.x() - 20, location.y() - 25, QString::fromStdString(id));
        break;
      }
      case 'd':
      {
        painter.setBrush(QColor(128, 128, 225, 192));
        painter.drawRect(location.x() - 30, location.y() - 20, 60, 40);
        painter.drawText(location.x() - 30, location.y() - 25, QString::fromStdString(id));
        break;
      }
    }
  }

  for (const Log& log : config.get_logs())
  {
    painter.setBrush(QColor(128, 62, 192, 192));

    const QPoint& location = log.get_location();

    QPainterPath path;
    path.moveTo(location.x() - 30, location.y() - 20);
    path.lineTo(location.x() + 30, location.y() - 20);
    path.lineTo(location.x(), location.y() + 20);
    path.closeSubpath();

    painter.drawPath(path);
    painter.fillPath(path, painter.brush());
    painter.drawText(location.x() - 30, location.y() - 25, "log");
  }

  for (const Log& log : config.get_logs())
  {
    for (Driver* const driver : log.get_drivers())
    {
      painter.drawLine(log.get_location(), driver->get_location());
    }
  }

  painter.end();
}
