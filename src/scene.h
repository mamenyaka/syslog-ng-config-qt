#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

#include <memory>

class Config;
class Object;
class Logpath;
class Icon;
class LogpathIcon;

class Scene : public QWidget
{
  Q_OBJECT

  Config& config;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  void add_object(std::shared_ptr<Object>& new_object, const QPoint& pos);
  void add_logpath(std::shared_ptr<Logpath>& new_logpath, const QPoint& pos);

  void reset();

protected:
  void leaveEvent(QEvent *);

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  QSize sizeHint() const;

private:
  void add_icon(Icon* icon, const QPoint& pos);
  void pressed(Icon* icon);
  void released(Icon* icon);
  LogpathIcon* select_nearest_logpath(const QPoint& pos) const;

  // non copyable
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;
};

#endif  // SCENE_H
