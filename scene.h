#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

class Config;
class DriverIcon;
class LogIcon;
class Driver;
class Log;
class QLabel;

class Scene : public QWidget
{
  Q_OBJECT

  Config& config;
  DriverIcon* selected_icon;

  QLabel* deleteLabel;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  void add_driver(Driver& driver, const QPoint& pos);
  void add_log(Log& log);

  void move_driver(const QPoint& pos);
  void delete_driver();

  void clear();
  void reset();

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  QSize sizeHint() const;

private:
  DriverIcon* select_nearest_driver() const;
  LogIcon* select_nearest_log(const QPoint& pos) const;
};

#endif  // SCENE_H
