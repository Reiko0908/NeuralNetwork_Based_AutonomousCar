# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Software/Espressif/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/tmp"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/src/bootloader-stamp"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/src"
  "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/PACMAIN/Project_C/esp_gamepad/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
