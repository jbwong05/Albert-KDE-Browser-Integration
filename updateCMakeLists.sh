#!/bin/bash
echo -e "\noption(BUILD_KDEBROWSERINTEGRATION \"Build the extension\" ON)\nif (BUILD_KDEBROWSERINTEGRATION)\n    add_subdirectory(Albert-KDE-Browser-Integration)\nendif()\n" >> ./CMakeLists.txt