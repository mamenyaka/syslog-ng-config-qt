#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Config;
class Driver;
class Log;

class Widget : public QWidget
{
  Q_OBJECT

  Config& config;

  Driver* selected_driver = nullptr;
  Log* selected_log = nullptr;
  bool log_selected_for_update = false;

  const double max_dist = 25.0;

public:
  explicit Widget(Config& config, QWidget* parent = 0);
  ~Widget();

  void set_selected_driver(Driver* driver);
  void set_selected_log(Log* log);
  void clear();

signals:
  void update_driver(Driver& driver);
  void update_statusbar(const std::string message);
  void clear_statusbar();

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent *);

private:
  Driver* get_nearest_driver(const QPoint& point);
  Log* get_nearest_log(const QPoint& point);
  void select_nearest(const QPoint& point);
};

#endif  // WIDGET_H
