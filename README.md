# syslog-ng-config-qt
A standalone graphical tool for creating syslog-ng configuration files

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
- read provided yaml file to create form with options for drivers
- create drivers and logs (buttons SRC, DST, LOG)
- update drivers and logs (double click to update)
- move drivers (click&hold to move)
- delete drivers (when moving, press Delete)
- Save button prints configuration to stdout
