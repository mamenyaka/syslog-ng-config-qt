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
- read provided yaml files to create form with options for source drivers, destination drivers, log paths, global options, filters, templates, rewrites, parsers
- create source drivers, destination drivers, filters, rewrites, parsers (drag&drop), log paths (Edit->New log), templates (Edit->New template)
- copy source drivers, destination drivers, global options, filters, templates, rewrites, parsers (hold down CTRL)
- update source drivers, destination drivers, log paths, filters, templates, rewrites, parsers (double click), global options (Edit->Global options) options
- move objects into and out of log paths (drag&drop)
- delete objects (move to delete icon)

