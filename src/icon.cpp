#include "icon.h"
#include "object.h"

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPalette>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#define ICON_SIZE 80

ObjectIcon::ObjectIcon(std::shared_ptr<Object>& object,
                       QWidget* parent) :
  QWidget(parent),
  object(std::move(object))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(this->object->get_name()), this);
  label->setFont(QFont("Sans", 8, QFont::DemiBold));
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
  mainLayout->addWidget(label);

  setupIcon();
}

std::shared_ptr<Object>& ObjectIcon::get_object()
{
  return object;
}

void ObjectIcon::paintEvent(QPaintEvent *)
{
  if (object->get_id() > -1)
  {
    QLabel* label = findChild<QLabel*>();
    label->setText("\n" + QString::fromStdString(object->get_name()) + "\n" + QString::number(object->get_id()));
  }
}

void ObjectIcon::setupIcon()
{
  QPixmap pixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  object->draw(&painter, width(), height());

  painter.end();

  QPalette palette;
  palette.setBrush(QPalette::Background, QBrush(pixmap));
  setPalette(palette);
}


LogpathIcon::LogpathIcon(std::shared_ptr<Logpath>& logpath,
                         QWidget* parent) :
  QWidget(parent),
  logpath(std::move(logpath))
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

std::shared_ptr<Logpath>& LogpathIcon::get_logpath()
{
  return logpath;
}

void LogpathIcon::add_object(ObjectIcon& icon)
{
  if (icon.get_object()->get_type() == "template")
  {
    return;
  }

  std::shared_ptr<Object>& object = icon.get_object();
  logpath->add_object(object);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->addWidget(&icon);

  icon.show();
  adjustSize();
}

void LogpathIcon::remove_object(ObjectIcon& icon)
{
  std::shared_ptr<Object>& object = icon.get_object();
  logpath->remove_object(object);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->removeWidget(&icon);

  frameLayout->activate();
  adjustSize();
}

bool LogpathIcon::eventFilter(QObject *, QEvent* event)
{
  event->ignore();
  return true;
}
