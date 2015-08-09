#ifndef ICON_H
#define ICON_H

#include <QWidget>

#include <memory>

class Object;
class Logpath;

class ObjectIcon : public QWidget
{
  Q_OBJECT

  std::shared_ptr<Object> object;

public:
  explicit ObjectIcon(std::shared_ptr<Object>& object,
                      QWidget* parent = 0);

  std::shared_ptr<Object>& get_object();

protected:
  void paintEvent(QPaintEvent *);

private:
  void setupIcon();
};

class LogpathIcon : public QWidget
{
  Q_OBJECT

  std::shared_ptr<Logpath> logpath;

public:
  explicit LogpathIcon(std::shared_ptr<Logpath>& logpath,
                       QWidget* parent = 0);

  std::shared_ptr<Logpath>& get_logpath();

  void add_object(ObjectIcon& icon);
  void remove_object(ObjectIcon& icon);

protected:
  bool eventFilter(QObject *, QEvent* event);

private:
  int get_index(ObjectIcon& icon);
};

#endif  // ICON_H
