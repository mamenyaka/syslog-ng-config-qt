#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

#include <memory>

class Config;
class Object;
class ObjectStatement;
class LogStatement;
class Icon;
class ObjectIcon;
class ObjectStatementIcon;
class ObjectStatementIconCopy;
class LogStatementIcon;

class Scene : public QWidget
{
  Q_OBJECT

  Config& config;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  ObjectIcon* add_object(std::shared_ptr<Object>& new_object, const QPoint& pos);
  ObjectStatementIcon* add_object_statement(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos);
  ObjectStatementIconCopy* add_object_statement_copy(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos);
  LogStatementIcon* add_log_statement(std::shared_ptr<LogStatement>& new_log_statement, const QPoint& pos);

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

  void check_for_copies(ObjectStatementIcon* icon);

  ObjectStatementIcon* select_nearest_object_statement_icon(const QPoint& pos) const;
  LogStatementIcon* select_nearest_log_statement_icon(const QPoint& pos) const;

  // non copyable
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;
};

#endif  // SCENE_H
