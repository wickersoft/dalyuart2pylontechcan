(This repo is still work in progress and won't work properly)


# Make your Daly BMS present as a name-brand battery pack
Hybrid solar inverters designed for a 48V battery pack are becoming increasingly popular and LFP cells are more affordable than ever. Despite this, most hybrid inverters are restrictive about the battery packs they will operate with, presumably for liability reasons. If you know what you're doing, you can safely build a battery pack of your own and get much more performance for your money. Commonly available BMS for LFP packs are not supported by solar inverters though. This project uses an Arduino compatible MCU to translate information from Daly brand BMS to the CAN protocol implemented by a widely supported range of battery modules made by Pylontech. This repo is based on a modified version of the [maland16/daly-bms-uart](https://github.com/maland16/daly-bms-uart) library.

# Installation
Simply clone, compile and flash the project. Adjust pinout (in the .ino file) and battery pack parameters (in the .h file) as desired. Check the validity of your BMS data by reading the debug information on the default serial port at 115200 baud before bringing your system online.

# Operation
The Arduino device is connected to the Daly BMS by two pins functioning as a SoftwareSerial, and to the solar inverter's CAN bus through a MCP2515 CAN transceiver. Information is automatically pulled from the Daly BMS every second and relayed to the inverter. This causes your DIY battery to be detected as an array of Pylontech US5000 battery packs, which are supported by most inverters. This communication allows the inverter to exchange power with your battery. Cell voltage, temperature, charge and discharge current limits and several error flags are forwarded to the inverter to allow safe operation of the battery.

# Tested with:
I'm only able to test the code with my own installation, which consists of a Daly R32W BMS (16S, 200A rating) and an IMEON 9.12 inverter. If you test this software, let me know how well it works for you.


# Deviations from direct data translation
The Imeon inverter makes a few decisions which go against the user's best interest. Through the CANBUS, the inverter can be fooled into doing the right thing.

## End of discharge
If the reported state of charge falls below the minimum SOC allowed in the Imeon settings or the "Pack Low Voltage" flag is set, the inverter will stop backfeeding until the battery is at 100%. To avoid this, the battery_health->soc_percent value is never reported below 12% and the Pack Low Voltage flag is never set. Instead, the max discharge current field is set to 1A. The safety of the battery is maintained by the Daly BMS, which is configured to disconnect the discharge current below 10%. This does not cause the inverter to become stingy with power.

## Abort a forced charge
The inverter may randomly decide the battery needs a force charge. In this state, backfeeding is also disabled until the battery reaches 100%. The inverter will also pull energy from the grid to accomplish this, with a configurable charge current. To cancel a forced charge, the project includes a GPIO pin which overrides the reported battery SOC as 100% until the inverter exits the "Forced charging" state.
Side note: There is a bug in the Imeon software that causes charge current limits to not regress to default after a reboot. In this situation, the inverter will draw 6.5kW from the grid to charge the battery at 120A.