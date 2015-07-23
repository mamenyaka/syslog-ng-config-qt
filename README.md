# syslog-ng-config-qt
A standalone graphical tool for creating syslog-ng configuration files

![Design]
(http://kepfeltoltes.hu/150721/782138767design_www.kepfeltoltes.hu_.png)

# Prerequisites
- Qt5
- yaml-cpp

# Bulding
```
qmake-qt5 syslog-ng-config-qt.pro
make
```

# Run
```
./syslog-ng-config-qt
```

# Current features
- read provided yaml files to create form with options for source driversm, destination drivers, global options, templates, filters
- create source driversm, destination drivers, filters (drag&drop), log paths (Edit->New log), templates (Edit->New template)
- copy source driversm, destination drivers, templates, filters (hold down CTRL)
- update source driversm, destination drivers, templates, filters (double click), log paths (drag&drop drivers), global options (Edit->Global options)
- delete source driversm, destination drivers, log paths, templates, filters (move to delete icon)

