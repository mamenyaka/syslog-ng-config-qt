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
  emit clear_statusbar();

  selected_driver = nullptr;
  selected_log = nullptr;
  log_selected_for_update = false;

  update();
}

void Widget::set_selected_driver(Driver* driver)
{
  clear();

  selected_driver = driver;

  update();
}

void Widget::set_selected_log(Log* log)
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
  if (log_selected_for_update)
  {
    Driver* const driver = get_nearest_driver(event->pos());

    if (driver != nullptr)
    {
      selected_log->has_driver(driver) ? selected_log->remove_driver(driver) : selected_log->add_driver(driver);

      update();
    }
  }
  else
  {
    if (selected_driver != nullptr)
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

      update();
    }
  }
}

void Widget::mouseDoubleClickEvent(QMouseEvent *)
{
  if (log_selected_for_update)
  {
    return;
  }
  else if (selected_driver != nullptr)
  {
    emit update_driver(*selected_driver);

    clear();
  }
  else if (selected_log != nullptr)
  {
    emit update_statusbar("Click to select/unselect drivers, press Esc when finished");

    log_selected_for_update = true;
  }
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
  if (log_selected_for_update)
  {
    return;
  }
  else if (selected_driver != nullptr)
  {
    selected_driver->set_location(event->pos());

    update();
  }
  else if (selected_log != nullptr)
  {
    selected_log->set_location(event->pos());

    update();
  }
}

void Widget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
  {
    clear();
  }

  if (log_selected_for_update)
  {
    return;
  }

  if (event->key() == Qt::Key_Delete)
  {
    if (selected_driver != nullptr)
    {
      config.erase_driver(selected_driver);

      clear();
    }
    else if (selected_log != nullptr)
    {
      config.erase_log(selected_log);

      clear();
    }
  }
}

void Widget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  for (const Driver& driver : config.get_drivers())
  {
    painter.setPen(&driver == selected_driver ? QPen(QBrush(Qt::black), 3) : QPen(QBrush(Qt::black), 1));

    const QPoint& location = driver.get_location();
    const std::string id = driver.print_id();

    const char type = driver.get_type().at(0);
    switch (type)
    {
      case 's':
      {
        painter.setBrush(QColor(255, 128, 128));
        painter.drawEllipse(location.x() - 20, location.y() - 20, 40, 40);
        painter.drawText(location.x() - 20, location.y() - 25, QString::fromStdString(id));
        break;
      }
      case 'd':
      {
        painter.setBrush(QColor(128, 128, 225));
        painter.drawRect(location.x() - 30, location.y() - 20, 60, 40);
        painter.drawText(location.x() - 30, location.y() - 25, QString::fromStdString(id));
        break;
      }
    }
  }

  for (const Log& log : config.get_logs())
  {
    painter.setPen(&log == selected_log ? QPen(QBrush(Qt::black), 3) : QPen(QBrush(Qt::black), 1));
    painter.setBrush(QColor(128, 62, 192));

    const QPoint& location = log.get_location();

    QPainterPath path;
    path.moveTo(location.x() - 30, location.y() - 20);
    path.lineTo(location.x() + 30, location.y() - 20);
    path.lineTo(location.x(), location.y() + 20);
    path.closeSubpath();

    painter.drawPath(path);
    painter.fillPath(path, painter.brush());
    painter.drawText(location.x() - 30, location.y() - 25,
                     QString::fromStdString("log_" + std::to_string(log.get_id())));
  }

  for (const Log& log : config.get_logs())
  {
    for (Driver* const driver : log.get_drivers())
    {
      painter.setPen(QPen(Qt::black));
      painter.drawLine(log.get_location(), driver->get_location());
    }
  }

  painter.end();
}
