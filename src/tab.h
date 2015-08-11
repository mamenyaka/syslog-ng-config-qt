#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <memory>

class Object;

class Tab : public QWidget
{
  Q_OBJECT

public:
  explicit Tab(QWidget* parent = 0);

  void setupObjects(const std::string& type, const std::vector< std::unique_ptr<const Object> >& default_objects);
};

#endif  // TAB_H
