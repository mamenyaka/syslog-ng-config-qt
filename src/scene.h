#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

#include <memory>

class Config;
class Icon;
class ObjectIcon;
class LogpathIcon;
class Object;
class Logpath;
class QLabel;

class Scene : public QWidget
{
  Q_OBJECT

  Config& config;

  QLabel* deleteLabel;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  void add_object(std::shared_ptr<Object>& new_object, const QPoint& pos);
  void add_logpath(std::shared_ptr<Logpath>& new_logpath, const QPoint& pos);

  void reset();

protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);

  void leaveEvent(QEvent *);

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  QSize sizeHint() const;

private:
  Icon* select_nearest_icon() const;
  ObjectIcon* select_nearest_object() const;
  LogpathIcon* select_nearest_logpath(const QPoint& pos) const;

  // non copyable
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;
};

#endif  // SCENE_H
