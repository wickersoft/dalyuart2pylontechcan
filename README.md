# Make your Daly BMS present as a name-brand battery pack 
Hybrid solar inverters designed for a 48V battery pack are becoming increasingly popular and LFP cells are more affordable than ever. Despite this, most hybrid inverters are restrictive about the battery packs they will operate with, presumably for liability reasons. If you know what you're doing, you can safely build a battery pack of your own and get much more performance for your money. Commonly available BMS for LFP packs are not supported by solar inverters though. This project uses an Arduino compatible MCU to translate information from Daly brand BMS to the CAN protocol implemented by a widely supported range of battery modules made by Pylontech. This repo is based on a modified version of the [maland16/daly-bms-uart](https://github.com/maland16/daly-bms-uart) library. 

# Installation
Simply clone, compile and flash the project. Adjust pinout (in the .ino file) and battery pack parameters (in the .h file) as desired. Check the validity of your BMS data by reading the debug information on the default serial port at 115200 baud before bringing your system online.

# Operation
The Arduino device is connected to the Daly BMS by two pins functioning as a SoftwareSerial, and to the solar inverter's CAN bus through a MCP2515 CAN transceiver. Information is automatically pulled from the Daly BMS every second and relayed to the inverter. This causes your DIY battery to be detected as an array of Pylontech US5000 battery packs, which are supported by most inverters. This communication allows the inverter to exchange power with your battery.

# Tested with:
I'm only able to test the code with my own installation,which consists of a Daly R32W BMS (16S, 200A rating) and an IMEON 9.12 inverter. If you test this software, let me know how well it works for you.
