# Circuit Project Notes: MPPT Current Output 

*Projeto Solares - Universidade Federal do Espirito Santo (UFES)*
*Author: João Agricio Lopes Barbosa*

## Table Of Contents
- [Circuit Project Notes: MPPT Current Output](#circuit-project-notes-mppt-current-output)
  - [Table Of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Utilized Components](#utilized-components)
    - [Hall Sensor CI](#hall-sensor-ci)
    - [Output Filters](#output-filters)
      - [ADC Impedance Characteristics](#adc-impedance-characteristics)
      - [ACS758 Output Impedance Restrictions](#acs758-output-impedance-restrictions)
      - [Filter Design Definition](#filter-design-definition)

## Introduction

This circuit needs to be able to read currents from 0 to 40 A theoretically. To accomplish this task we intend to use a hall sensor. The choice of the sensor can be based on the smaller footprint (in comparison with other alternatives, _i.e_ like shunt resistors).

## Utilized Components

### Hall Sensor CI

The hall sensor choosed for this project is the ACS758LCB-050U ([datasheet ACS758](https://github.com/Collab-Barco-Solar/poente-boards/blob/develop/electric_project/telemetria_hardware/2020/Base%20Material/Current%20Sensing/Current%20from%20MPPT/ACS758-Datasheet.pdf)). The ACS758 is an unidirectional hall sensor with an analog output ranging ratiometrically to the voltage supply. From datasheet, the minimal output resistance is 4.7 k<span>&#8486;</span>. Some capital caracteristics can be listed:

1. Care must be taken about trace widths in view of the high current input;
2. Some empirical adjusts will be necessary in view of the 3.3 V supply (not standard in the datasheet);
3. In view of the transient nature of the MPPT output current.

### Output Filter

The filter circuit is a sensitive design and a lot a thought must be putted on. The RC input impedance of the filter can interfere and cause reading errors on the ADC. So, a good practice is to maintain the RC impedance as low as possible (witch can cause high output current from the sensor) and to achieve this goal  a balance must be made between the lowest possible impedance and the lowest dissipated power.

#### ADC Impedance Characteristics

In the ESP32 Datasheet ([ESP32 Datasheet](https://github.com/Collab-Barco-Solar/poente-boards/blob/develop/electric_project/telemetria_hardware/2020/Base%20Material/datasheets/MCU's/esp32_datasheet_en.pdf)) there's no information about the input impedance. So, the approach will be to make the RC filter impedance as low as possible.

#### ACS758 Output Impedance Restrictions

In the [ACS758 datasheet](https://github.com/Collab-Barco-Solar/poente-boards/blob/develop/electric_project/telemetria_hardware/2020/Base%20Material/Current%20Sensing/Current%20from%20MPPT/ACS758-Datasheet.pdf) some impedance parameters are described.

|Symbol                |Value                     |
| -------------------- | ------------------------:|
|C<sub>load (max)</sub>| 10 nF                    |
|R<sub>load (min)</sub>| 4.7 k<span>&#8486;</span>|

#### Filter Design Definition

Thus, based on the ACS758 impedance boundaries the filter must have a resistance equal or greater than 4.7 k<span>&#8486;</span> which implies in a limitation in the rate of readings from the ADC (SAR's ADC's). To mantaing this limitation as low as possible, the value of the resistor will be 4.7 k<span>&#8486;</span>. Based on the transient caracteristic of the load and for convenience (capacitor values alredy on the project) the filter cut-off frequency will be 338.63 Hz, achieved using a 100 nF capacitor.

## Wire-To-Board Connector

The chosen connector is commonly used in Remote Control Flying Drones (RC drones). A bullet kind connector that has a rated current of 30 A and maximum current of 60 A. The model is the XT60 (specification can be found in [datasheet](https://github.com/Collab-Barco-Solar/poente-boards/blob/develop/electric_project/telemetria_hardware/2020/Base%20Material/Current%20Sensing/Current%20from%20MPPT/Connector%20XT60/XT60%20SPEC.pdf)). The rated current probably won't be a problem since in normal operation the MPPT output current is less than 30 A and the maximum output current is 45 A.

## PCB Design Precautions

In view of the great maximum current that can go through the PCB, some caution must be taken. The main factors are due to heat dissipation and generation. Being so, taking in view that the heat generation is caused by the intrinsic resistance of the rails and connections, effort should be applied to make good connections and to make the rails as wide and as thick as possible.

It's highly recommended the use of some PCB calculation software (like the [Saturn PCB Design Toolkit](http://saturnpcb.com/pcb_toolkit/)) to obtain reliable results about ideal trace width and heat generated. It can save some time of math.