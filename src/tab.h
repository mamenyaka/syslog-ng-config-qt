#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <memory>

class Driver;
class DriverIcon;

class Tab : public QWidget
{
  Q_OBJECT

public:
  explicit Tab(QWidget* parent = 0);

  void setupDrivers(const std::string& type, const std::vector< std::unique_ptr<const Driver> >& default_drivers);

protected:
  void mousePressEvent(QMouseEvent *);

private:
  DriverIcon* select_nearest_driver() const;
};

#endif  // TAB_H
