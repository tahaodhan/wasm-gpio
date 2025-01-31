# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/todhan/esp/v5.4/esp-idf/components/bootloader/subproject"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/tmp"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/src/bootloader-stamp"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/src"
  "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/todhan/esp32/testing-functions/wasm-gpio/rgb-led-wasm/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
