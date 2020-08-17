# Circuit Project Notes: MPPT Current Output 


*Author: João Agricio Lopes Barbosa*

## Introduction
This circuit needs to be able to read currents from 0 to 40 A theoretically. To accomplish this task we intend to use a hall sensor. The choice of the sensor can be based on the smaller footprint (in comparison with other alternatives, _i.e_ like shunt resistors).

## Utilized Components

### Hall Sensor CI
The hall sensor choosed for this project is the ACS758LCB-050U ([datasheet ACS758](https://github.com/Collab-Barco-Solar/poente-boards/blob/develop/electric_project/telemetria_hardware/2020/Base%20Material/Current%20Sensing/Current%20from%20MPPT/ACS758-Datasheet.pdf)). The ACS758 is an unidirectional hall sensor with an analog output ranging ratiometrically to the voltage supply. Some capital caracteristics can be listed:

1. Care must be taken about trace widths in view of the high current input;
2. Some empirical adjusts will be necessary in view of the 3.3 V supply (not standard in the datasheet);
3. In view of the transient nature of the MPPT output current

### Output filters
The filter circuit is a sensitive design and a lot a thought must be putted on. The RC input impedance of the filter can interfere and cause reading errors on the ADC. So, a good practice is to mantain the RC impedance as low as possible (witch can cause high output current from the sensor) and to achieve this goal  a balance must be made between the lowest possible impedance and the lowest dissipated power.

