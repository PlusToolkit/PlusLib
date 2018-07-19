This module allow you to use the Infrared Thermal Seek Pro Camera.

You have to execute install.sh with sudo privileges to grant the privileges to access to the camera in Linux.

./sudo install.sh

Or install the driver WinUSB to the camera in Windows. We recommend using the Zaing tool available at http://zadig.akeo.ie/.

The camera configuration file may include two fields called "FlatFile" and "BiasFile" with the path to those calibration files. If the boolean "CalibrateTemperature" field is set to true, the user must include two decimal fields, "Temperaturemultiplier" and "TemperatureBias" which will be applied to each pixel using the operation:

value = value x TemperatureMultiplier - TemperatureBias

Then, you can use the Infrared Thermal Seek Pro module to use the camera.


Developed by MACBIOIDI-ULPGC & IACTEC-IAC group

The MIT License (MIT)

Copyright (c) 2017 Eavise Research Group

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
