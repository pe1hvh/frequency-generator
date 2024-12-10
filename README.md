10khz to 225mhz vfo rf generator
==================================

  Original source: https://create.arduino.cc/projecthub/CesarSound/10khz-to-225mhz-vfo-rf-generator-with-si5351-version-2-bfa619?ref=user&ref_id=1465945&offset=1
  10kHz to 225MHz VFO / RF Generator with Si5351 and Arduino Nano, with Intermediate Frequency (IF) offset
  (+ or -), RX/TX Selector for QRP Transceivers, Band Presets and Bargraph S-Meter. See the schematics for
  wiring and README.txt for details. By J. CesarSound - ver 2.0 - Feb/2021. 
  
  Forked by PE1HVH for a project done by the de VERON Afdeling 49/VRZA Afdeling 22.
  
  Changes   
  -------
  Version 3.0 december 2024
   * complete new redesign of the code.
   * using namespaces and build classes to give this project a clear structure
   
  PlatformIO
  ----------
  While using PlatformIO with Visual Code as IDE, I have to re-order the function.
  
  PlatformIO requires that functions be declared before they are called in the code. This is because PlatformIO uses a more standard C++ compilation process,
  which adheres strictly to the requirement that functions must be declared before use.

  SonarLint
  ---------
  For writing cleancode, the Sonarlint plug-in is installed in Visual Code. 
  SonarLint is an open-source code analysis tool that serves several important functions to help developers write cleaner, safer, and higher-quality code. 
  There is no need, for using all sonarlint rules but the most important are enabled.
  
  Compatible
  ----------
  To avoid compatibility issues, the source is saved as an arduino.ino file
  

  
