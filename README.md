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
- read provided yaml files to create form with options for drivers
- create drivers (drag&drop)
- update drivers (double click)
- create logs (Edit->New log)
- update logs (drag&drop drivers)
- update global options (Edit->Global options)
- delete drivers (move to delete icon)
