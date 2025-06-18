# PCB Business Card
<img src="/PCB_Design/Business_Card/PCB_ISOView.png" width="800">

[![Hardware License: CERN-OHL-S-2.0](https://img.shields.io/badge/Hardware%20License-CERN--OHL--S--2.0-lightgrey.svg)](https://ohwr.org/cern_ohl_s_v2.txt)
[![Software License: GPL v3](https://img.shields.io/badge/Software%20License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Status: In Development](https://img.shields.io/badge/Status-In%20Development-yellow.svg)]()


<!-- ABOUT -->
## About The Project
A business card with embedded IPS display, SMD pushbuttons, and a XIAO nRF5240 Sense designed to run off a slim LiPo Battery.
The goal of this PCB is to create an artistic business card for conventions, maker spaces, and other social events that stands out either assembled or unassembled for portability.


<!-- REQUIREMENTS -->
### Design Tools & Requirements

#### Software Requirements
- VS Code / Arduino (Programming IDE)
- Autodesk Fusion360 (Free CAD Software)
- KiCAD 9.0 (Open Source PCB Design Software)
- Ultimaker Cura (Free 3D Slicing Software)
- QR Code Generator

#### Bill of Materials
- Custom PCBs
- MCU: XIAO nRF5240 Sense
- Low Profile SMD Momentary Pushbutton: TL3342
- LIPO Connector
- LIPO Battery
- 1.14" 135x240 RGB IPS LCD SPI Display with ST7789V driver IC.

<!-- POWER -->
### Component Power Draw
- XIAO nRF5240 Sense:
- IPS Display:
- Total:
- With Safety Factor (x1.25):
- Recommended Lipo: 
- Estimated Battery Life:

<!-- PCB Design -->
### PCB Design
<img src="/PCB_Design/Business_Card/PCB_TopView.png" width="800">
<img src="/PCB_Design/Business_Card/PCB_BottomView.png" width="800">


This PCB is used to interface a XIAO nRF5240 Sense with the following components:
- SMD Pushbuttons
- IPS Display
- Power Source (Lipo Battery)

I did some research on artistic designs and found people using soldermask as a visual aesthetic. When the board is finished with ENIG, it produces a protected and nice gold finish.
For the board thickness I went with 0.8mm. (Typical board is 1.6mm). For future revisions, I may make the board even thinner.
The end goal is not use a breakout board and to simply have the nRF5240 or another board like the ESP32-S3 bare chip used along with battery charging and microphone circuits to reduce the physical space taken up. 

<!-- Software -->
### Software Design
With 4x buttons available along with a microphone input, a variety of things can be displayed on the screen.
I wanted to incorporate a menu system so I can toggle between different modes such as a battery readout display, static image display such as for a QR code, a looping GIF display, a sound reactive display mode, and a control mode to set ambient audio input levels. The limitation of all of this is the amount of flash memory on the XIAO nRF5240. For future revisions, I can incorporate a microSD card.

#### Menu Display
TBD

#### Battery Readout Display
TBD

#### Static Image Display
TBD

#### Looping GIF Display
TBD

#### Sound Reactive Display
TBD

#### Ambient Audio Config Display
TBD

#### Game Display
TBD

<!-- CONTRIBUTING -->
## Contributing & Donations

Any donation helps to support parts for future open source projects!
[Buy Me a Coffee ☕](https://www.buymeacoffee.com/rganlath)

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!


<!-- LICENSE -->
## License
The contents of this project itself is licensed under CERN-OHL-S-2.0 and the underlying source code is licensed under GNU GPLv3. Both of these licenses require attribution.
