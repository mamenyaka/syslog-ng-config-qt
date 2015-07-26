#include "icon.h"
#include "config.h"

#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#include <cmath>

#define ICON_SIZE 80

DriverIcon::DriverIcon(std::shared_ptr<Driver>& driver,
                       QWidget* parent) :
  QWidget(parent),
  driver(std::move(driver))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(this->driver->get_name()), this);
  label->setFont(QFont("Sans", 8, QFont::DemiBold));
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
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
  if (driver->get_id() > -1)
  {
    QLabel* label = findChild<QLabel*>();
    label->setText("\n" + QString::fromStdString(driver->get_name()) + "\n" + QString::number(driver->get_id()));
  }
}

void DriverIcon::setupIcon()
{
  pixmap = QPixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  switch (driver->get_type())
  {
    case DriverType::source:
    {
      painter.setBrush(QColor(255, 128, 128, 192));
      painter.drawEllipse(1, 1, width() - 2, height() - 2);
      break;
    }
    case DriverType::destination:
    {
      painter.setBrush(QColor(128, 128, 225, 192));
      painter.drawRect(1, 1, width() - 2, height() - 2);
      break;
    }
    case DriverType::filter:
    {
      painter.setBrush(QColor(128, 255, 128, 192));

      QPainterPath path;
      path.moveTo(width()/2.0, 1);
      path.lineTo(width() - 1, height()/2.0);
      path.lineTo(width()/2.0, height() - 1);
      path.lineTo(1, height()/2.0);
      path.closeSubpath();

      painter.drawPath(path);
      painter.fillPath(path, painter.brush());
      break;
    }
    case DriverType::template_:
    {
      painter.setPen(QColor(255, 128, 255, 192));
      painter.drawEllipse(1, 1, width() - 2, height() - 2);
      break;
    }
    case DriverType::rewrite:
    {
      painter.setBrush(QColor(128, 255, 255, 192));

      const double pi = std::acos(-1);
      double h = (1 - std::cos(pi/6.0))*(height() - 2)/2;

      QPainterPath path;
      path.moveTo(width()/2.0, 1);
      path.lineTo(width() - 1 - h, 1 + (height() - 2)*1/4.0);
      path.lineTo(width() - 1 - h, 1 + (height() - 2)*3/4.0);
      path.lineTo(width()/2.0, height() - 1);
      path.lineTo(1 + h, 1 + (height() - 2)*3/4.0);
      path.lineTo(1 + h, 1 + (height() - 2)*1/4.0);
      path.closeSubpath();

      painter.drawPath(path);
      painter.fillPath(path, painter.brush());
      break;
    }
    case DriverType::parser:
    {
      painter.setBrush(QColor(255, 16, 32));
      painter.drawEllipse(1 + width()*1/4.0, 1, (width() - 2)/2.0, (height() - 2)/2.0);
      painter.setBrush(QColor(0, 128, 32));
      painter.drawEllipse(1 + width()*1/4.0, height()/2.0, (width() - 2)/2.0, (height() - 2)/2.0);
      painter.setBrush(QColor(64, 128, 255));
      painter.drawEllipse(1, 1 + (height() - 2)*1/4.0, (width() - 2)/2.0, (height() - 2)/2.0);
      painter.setBrush(QColor(255, 192, 16));
      painter.drawEllipse(width()/2.0, 1 + (height() - 2)*1/4.0, (width() - 2)/2.0, (height() - 2)/2.0);
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

Log& LogIcon::get_log()
{
  return *log;
}

void LogIcon::add_driver(DriverIcon& icon)
{
  if (icon.get_driver().get_type() == DriverType::template_)
  {
    return;
  }

  std::shared_ptr<Driver>& driver = icon.get_driver_ptr();
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
