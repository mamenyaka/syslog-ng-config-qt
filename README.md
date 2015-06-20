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
- read provided yaml file
- create drivers, set options (buttons SRC and DST)
- update drivers (double click to update)
- move drivers (click to move, click again to release)
- delete drivers (work in progress) (right click to delete)
