# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/bert/esp/esp-idf/components/bootloader/subproject"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/tmp"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/src/bootloader-stamp"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/src"
  "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/bert/Documents/EEH/HEE-FOG/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
