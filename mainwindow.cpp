#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include "dialog.h"

#include <QMessageBox>
#include <QInputDialog>

#include <yaml-cpp/yaml.h>

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionAbout, &QAction::triggered, [=]() {
    QMessageBox::aboutQt(this);
  });

  widget = new Widget;
  ui->horizontalLayout->addWidget(widget);

  connect(this, &MainWindow::create_shape, widget, &Widget::add_shape);

  connect(widget, &Widget::driver_added, [=]() {
    ui->statusBar->clearMessage();
  });
  connect(widget, &Widget::update_driver, [=](const std::string& id) {
    Driver& driver = *get_driver_iterator(id);
    form_dialog(driver);
  });
  connect(widget, &Widget::delete_driver, [=](const std::string& id) {
    drivers.erase(get_driver_iterator(id));
  });

  connect(ui->actionSource, &QAction::triggered, [=]() {
    driver_select_dialog("source");
  });
  connect(ui->actionDestination, &QAction::triggered, [=]() {
    driver_select_dialog("destination");
  });

  parse_yaml();
}

MainWindow::~MainWindow()
{
  delete ui;
  delete widget;
}

void MainWindow::driver_select_dialog(const std::string& type)
{
  QStringList items;
  for (const DefaultDriver& driver : default_drivers)
  {
    if (driver.get_type() == type)
    {
      items << QString::fromStdString(driver.get_name());
    }
  }

  if (items.isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"), tr("No drivers found!"));
    return;
  }

  bool ok;
  const std::string& name = QInputDialog::getItem(this, tr("Create new"), QString::fromStdString("Select " + type),
                                                  items, 0, false, &ok).toStdString();

  if (ok && !name.empty())
  {
    Driver driver = *get_default_driver_iterator(name, type);
    driver.set_id(get_count(name, type));

    int exec = form_dialog(driver);

    if (exec == QDialog::Accepted)
    {
      drivers.push_back(driver);

      emit create_shape(driver.get_id());

      ui->statusBar->showMessage(QString::fromStdString("Click to add " + driver.get_id()));
    }
  }
}

int MainWindow::form_dialog(Driver& driver)
{
  Dialog dialog(driver, this);

  return dialog.exec();
}

std::vector<DefaultDriver>::const_iterator MainWindow::get_default_driver_iterator(const std::string& name, const std::string& type) const
{
  for (std::vector<DefaultDriver>::const_iterator it = default_drivers.cbegin(); it != default_drivers.cend(); ++it)
  {
    const DefaultDriver& driver = *it;
    if (driver.get_name() == name && driver.get_type() == type)
    {
      return it;
    }
  }

  return default_drivers.cend();
}

std::vector<Driver>::iterator MainWindow::get_driver_iterator(const std::string& id)
{
  for (std::vector<Driver>::iterator it = drivers.begin(); it != drivers.end(); ++it)
  {
    const Driver& driver = *it;
    if (driver.get_id() == id)
    {
      return it;
    }
  }

  return drivers.end();
}

int MainWindow::get_count(const std::string& name, const std::string& type) const
{
  int count = 0;
  for (const Driver& driver : drivers)
  {
    if (driver.get_name() == name && driver.get_type() == type)
    {
      count++;
    }
  }

  return count;
}

void MainWindow::parse_yaml()
{
  const YAML::Node root = YAML::LoadFile("config.yml");
  for (YAML::const_iterator driver_it = root.begin(); driver_it != root.end(); ++driver_it)
  {
    const YAML::Node& yaml_driver = driver_it->second;

    const std::string name = yaml_driver["name"].as<std::string>();
    const std::string type = yaml_driver["type"].as<std::string>();
    const std::string description = yaml_driver["description"] ? yaml_driver["description"].as<std::string>() : "";
    const YAML::Node& options = yaml_driver["options"];

    DefaultDriver driver(name, type, description);

    for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
    {
      YAML::const_iterator tmp = option_it->begin();
      const YAML::Node& yaml_option = tmp->second;

      const std::string name = tmp->first.as<std::string>();
      const std::string type = yaml_option["type"].as<std::string>();
      const std::string description = yaml_option["description"] ? yaml_option["description"].as<std::string>() : "";
      const std::string default_value = yaml_option["default"] ? yaml_option["default"].as<std::string>() : "";

      Option option(name, type, description, default_value);

      if (type == "list" || type == "set")
      {
        const YAML::Node& values = yaml_option["values"];
        for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
        {
          const std::string value = value_it->as<std::string>();
          option.add_value(value);
        }
      }

      driver.add_option(option);
    }

    default_drivers.push_back(std::move(driver));
  }
}
