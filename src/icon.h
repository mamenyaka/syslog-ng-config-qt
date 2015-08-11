#ifndef ICON_H
#define ICON_H

#include <QWidget>

#include <memory>

class Object;
class Logpath;

class Icon : public QWidget
{
  Q_OBJECT

public:
  explicit Icon(QWidget* parent = 0);
  ~Icon() {}

signals:
  void pressed(Icon* icon);
  void released(Icon* icon);

protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  virtual void mouseDoubleClickEvent(QMouseEvent *) = 0;
};

class ObjectIcon : public Icon
{
  Q_OBJECT

  std::shared_ptr<Object> object;

public:
  explicit ObjectIcon(std::shared_ptr<Object>& object,
                      QWidget* parent = 0);

  std::shared_ptr<Object>& get_object();

protected:
  void mouseDoubleClickEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

private:
  void setupIcon();
};

class LogpathIcon : public Icon
{
  Q_OBJECT

  std::shared_ptr<Logpath> logpath;

public:
  explicit LogpathIcon(std::shared_ptr<Logpath>& logpath,
                       QWidget* parent = 0);

  void add_object(ObjectIcon& icon);
  void remove_object(ObjectIcon& icon);

protected:
  void mouseDoubleClickEvent(QMouseEvent* event);

private:
  int get_index(ObjectIcon& icon);
};

class DeleteIcon : public QWidget
{
  Q_OBJECT

public:
  explicit DeleteIcon(QWidget* parent = 0);
};

#endif  // ICON_H
