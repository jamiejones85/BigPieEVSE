# BigPieEVSE
Arduino code for a super simple EVSE, intended for development use only.
- Generates the Control Pilot signal and responds to EV connection
- Very Very simple code
- Very Very simple hardware
- Use at own risk
- Doesn't currently close any relays to enable AC, easy to add.
- No GFI
- No Proximity Pilot

Based on https://www.instructables.com/Arduino-EV-J1772-Charging-Station/

Uses a cheap +-12v DCDC module and an OPAMP.

https://www.ebay.co.uk/itm/Negative-Voltage-Dual-DC12V-12V-Power-Supply-Module-5-12V-to-12V-For-Amplifier/192115084655?hash=item2cbaf38d6f:g:BZQAAOSwdGFYtCPN

Control Pilot Signal Components
- LF353
- P6KE16CA bidirectional TVS diode
- 1% 1k ohm resistor
- 100k ohm resistor * 3
- 56k ohm resistor
- 200k ohm resistor
- +12 & -12v power supply module
