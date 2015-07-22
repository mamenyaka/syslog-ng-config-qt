#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

#include <memory>

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
  DriverIcon* selected_icon = nullptr;
  bool copy_icon = false;

  QLabel* deleteLabel;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  void add_driver(std::shared_ptr<Driver>& new_driver, const QPoint& pos);
  void add_log(std::unique_ptr< Log, std::function<void(const Log *)> >& new_log);

  void move_driver(const QPoint& pos);

  void clear();
  void reset();

protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent *);

  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);

  void leaveEvent(QEvent *);

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  QSize sizeHint() const;

private:
  DriverIcon* select_nearest_driver() const;
  LogIcon* select_nearest_log(const QPoint& pos) const;
};

#endif  // SCENE_H
