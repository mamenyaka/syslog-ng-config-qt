#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <memory>

class Object;
class ObjectIcon;

class Tab : public QWidget
{
  Q_OBJECT

public:
  explicit Tab(QWidget* parent = 0);

  void setupObjects(const std::string& type, const std::vector< std::unique_ptr<const Object> >& default_objects);

protected:
  void mousePressEvent(QMouseEvent *);

private:
  ObjectIcon* select_nearest_object() const;
};

#endif  // TAB_H
