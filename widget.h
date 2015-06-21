#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <vector>

class Driver;

class Widget : public QWidget
{
  Q_OBJECT

  std::vector<Driver>& drivers;
  Driver* selected_driver = nullptr;

public:
  explicit Widget(std::vector<Driver>& drivers, QWidget* parent = 0);
  ~Widget();

public slots:
  void add_driver(Driver* driver);

signals:
  void update_driver(Driver& driver);

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent *);

private:
  Driver* get_nearest_driver(const QPoint& point);
};

#endif  // WIDGET_H
