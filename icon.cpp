#include "icon.h"
#include "config.h"

#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#define ICON_SIZE 80

DriverIcon::DriverIcon(std::shared_ptr<Driver>& driver,
                       QWidget* parent) :
  QWidget(parent),
  driver(std::move(driver))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(this->driver->get_name()), this);
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(0);
  mainLayout->addWidget(label);

  setupIcon();

  QPalette palette;
  palette.setBrush(QPalette::Window, QBrush(pixmap));
  setPalette(palette);
}

Driver& DriverIcon::get_driver()
{
  return *driver;
}

std::shared_ptr<Driver>& DriverIcon::get_driver_ptr()
{
  return driver;
}

const QPixmap& DriverIcon::get_pixmap() const
{
  return pixmap;
}

void DriverIcon::paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  if (driver->get_id() > -1)
  {
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setPen(QPen(Qt::black));
    painter.drawText(0, height() - 20, width(), 20, Qt::AlignHCenter, QString::number(driver->get_id()));
  }

  painter.end();
}

void DriverIcon::setupIcon()
{
  pixmap = QPixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  QRect rect(1, 1, width() - 2, height() - 2);

  switch (driver->get_type())
  {
    case DriverType::source:
    {
      painter.setBrush(QColor(255, 128, 128, 192));
      painter.drawEllipse(rect);
      break;
    }
    case DriverType::destination:
    {
      painter.setBrush(QColor(128, 128, 225, 192));
      painter.drawRect(rect);
      break;
    }
    case DriverType::template_:
    {
      painter.setPen(QColor(255, 128, 255, 192));
      painter.drawEllipse(rect);
      break;
    }
    case DriverType::filter:
    {
      painter.setBrush(QColor(128, 255, 128, 192));

      QPainterPath path;
      path.moveTo(width()/2, 1);
      path.lineTo(width() - 1, height()/2);
      path.lineTo(width()/2, height() - 1);
      path.lineTo(1, height()/2);
      path.closeSubpath();

      painter.drawPath(path);
      painter.fillPath(path, painter.brush());
      break;
    }
    case DriverType::rewrite:
    {
      painter.setBrush(QColor(128, 255, 255, 192));

      double h = (1 - cos(3.14/6.0))*height()/2.0;

      QPainterPath path;
      path.moveTo(1 + width()*1.0/4.0, 1 + h);
      path.lineTo(1 + width()*3.0/4.0, 1 + h);
      path.lineTo(width() - 1, height()/2);
      path.lineTo(1 + width()*3.0/4.0, height() - 1 - h);
      path.lineTo(1 + width()*1.0/4.0, height() - 1 - h);
      path.lineTo(1, height()/2);
      path.closeSubpath();

      painter.drawPath(path);
      painter.fillPath(path, painter.brush());
      break;
    }
  }

  painter.end();
}


LogIcon::LogIcon(LogUPtr& log,
                 QWidget* parent) :
  QWidget(parent),
  log(std::move(log))
{
  installEventFilter(this);

  QLabel* label = new QLabel("L\nO\nG");
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QFrame* frame = new QFrame;
  frame->setLineWidth(2);
  frame->setFrameStyle(QFrame::Box | QFrame::Plain);
  frame->setMinimumSize(ICON_SIZE, ICON_SIZE);

  QVBoxLayout* frameLayout = new QVBoxLayout(frame);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(label);
  mainLayout->addWidget(frame);
}

void LogIcon::add_driver(DriverIcon& icon)
{
  std::shared_ptr<const Driver> driver = icon.get_driver_ptr();
  log->add_driver(driver);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->addWidget(&icon);

  icon.show();
  adjustSize();
}

void LogIcon::remove_driver(DriverIcon& icon)
{
  std::shared_ptr<Driver>& driver = icon.get_driver_ptr();
  log->remove_driver(driver);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->removeWidget(&icon);

  frameLayout->activate();
  adjustSize();
}

bool LogIcon::eventFilter(QObject *, QEvent* event)
{
  event->ignore();
  return true;
}
