# Albert KDE Browser Integration

## Description
Port of the KRunner code from the [Plasma Browser Integration Project](https://invent.kde.org/plasma/plasma-browser-integration) to a QPlugin for the [Albert Launcher](https://albertlauncher.github.io/) allowing [Albert](https://albertlauncher.github.io/) to access and switch to open browser tabs

## Dependencies
This plugin shares the same dependencies that are needed to build [Albert](https://albertlauncher.github.io/) from sources. Information about building [Albert](https://albertlauncher.github.io/) from sources and its dependencies can be found [here](https://albertlauncher.github.io/docs/installing/). This plugin also requires the [Plasma Browser Integration](https://community.kde.org/Plasma/Browser_Integration) extension to be installed in order to function properly. Information on installing this can be found [here](https://community.kde.org/Plasma/Browser_Integration).

## Installation

### Option 1: Compile alongside Albert
```
git clone --recursive https://github.com/albertlauncher/albert.git
cd albert/plugins
git submodule add https://github.com/jbwong05/Albert-KDE-Browser-Integration
./Albert-KDE-Browser-Integration/updateCMakeLists.sh
cd ..
mkdir albert-build
cd albert-build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

### Option 2: Compile separately from Albert
```
git clone https://github.com/jbwong05/Albert-KDE-Browser-Integration.git
cd Albert-KDE-Browser-Integration
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_SEPARATELY=ON ..
make
sudo make install
```

### Uninstallation
```
sudo rm -f /usr/lib/albert/plugins/libkdebrowserintegration.so
```

## Credits
The main logic of this plugin was based on the [tabsrunner plugin](https://invent.kde.org/plasma/plasma-browser-integration/-/tree/master/tabsrunner) from the [Plasma Browser Integration Project](https://invent.kde.org/plasma/plasma-browser-integration).
