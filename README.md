## This IS Very WIP!!!


New! Video [here](https://www.youtube.com/watch?v=-zq66LNntuQ)

## What is Ambii?
Its short for [Ambisonics](https://en.wikipedia.org/wiki/Ambisonics) Which is a [Binaural Microphone](https://en.wikipedia.org/wiki/Binaural_recording) on sterioids- Kind of...

This repo includes the code for the recorder, since I like to make stuff as cheap as I can, and quad input recording devices are mad expensive, and ohh you want to include portibility in the search? haa.. ha.

this project includes everything, including the 3d printable files to print the actual mic too. avalable [here](https://www.thingiverse.com/thing:7132631)
and the firmware [here](https://github.com/ICantMakeThings/Ambii)

okay okay enough, components,

## Required Components

- **ESP32-S3 Mini**: [AliExpress Link](https://aliexpress.com/item/1005005449219195.html)
- **PCM1808 (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_oCrSlXR)
- **Aux-M (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_o2d4zcZ)
- **Aux-F (need 4pcs)**: [AliExpress Link](https://aliexpress.com/item/1005005895047424.html)
- **Microphone Capsule (need 4pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_oltEmLb)
- **SD Card**: [AliExpress Link](https://s.click.aliexpress.com/e/_oFsbpBB)
- **SD Card Holder**: [AliExpress Link](https://s.click.aliexpress.com/e/_olg0Y8z)
- **OLED Display**: [AliExpress Link](https://s.click.aliexpress.com/e/_olLPabx)
- **Mic Preamp (maybe) (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_o2CUG6t)
- Some thin wires to connect the parts together

*Note: Some of thease are referral links. If you purchase through it, I earn a commission at no extra cost to you.*

<img width="756" height="1056" alt="image" src="https://github.com/user-attachments/assets/4c328860-04c7-42c4-8dbf-af1fde188d27" />
<img width="1498" height="792" alt="image" src="https://github.com/user-attachments/assets/a51f8005-05e2-4829-aaa7-02201cd38d1c" />

<img width="1819" height="991" alt="image" src="https://github.com/user-attachments/assets/71df9c74-bb81-427d-ac3e-1ceb55fa1cc0" />
<img width="1707" height="995" alt="image" src="https://github.com/user-attachments/assets/417cd24c-3441-4a24-96cc-81c0b38045fd" />


<img width="1190" height="845" alt="SCH_Ambii Schematic_1" src="https://github.com/user-attachments/assets/529a0414-6883-4752-a784-85976aa106a2" />

Schem on Oshwlab [here](https://oshwlab.com/nottoenail/ambii)

The Wiring is done in such a way where most of the pins are soldered with pins with a little gap:

<img width="1094" height="827" alt="image" src="https://github.com/user-attachments/assets/e456e050-0f12-46bd-a41e-4a657a8f8936" />

I soldered the pins to the PCM, then removed the plastic spacers and soldered the S3 on top, 

Only BCK has a wire to a gpio since it didnt fit, on both PCM's you want to short PMT, MD1, MD0 with GND, 3v to 3v and 5v to 5v. 

As of rn, the Mic Preamps havent arrived, I assume they work but we will see, 

#### pinouts:

| SD CARD   | ESP32-S3 Pin |
| -------- | ------------ |
| **CS**   | GPIO 33      |
| **MOSI** | GPIO 21      |
| **MISO** | GPIO 5       |
| **SCK**  | GPIO 17      |

| OLED DISPLAY  | ESP32-S3 Pin |
| ------- | ------------ |
| **SDA** | GPIO 11      |
| **SCL** | GPIO 10      |


| PCM 1808  | Pin on PCM     | ESP32-S3 Pin |
| --------- | ---------- | ------------ |
| **PCM-1** | SCK  | GPIO 35      |
|     **PCM-1**       | BCK        | GPIO 38      |
|     **PCM-1**       | LRC   | GPIO 18      |
|     **PCM-1**       | OUT | GPIO 16      |
| **PCM-2** | SCK | GPIO 13      |
|      **PCM-2**     | BCK        | GPIO 2       |
|     **PCM-2**      | LRC    | GPIO 12      |
|     **PCM-2**      | OUT | GPIO 4       |



It records *24-bit PCM @ 48 kHz* With a possibility to bump that up to 96 kHz, but dont think I will.

also for it to work, in my images you see the capsules are poking out of the filet, you want the mic capsule to be flush with the 3D model

There is a 14.6mm hole Version for [this capsule](https://micbooster.com/product/primo-em204n-cardioid-electret-microphone)

###### Only interested in 2 channels? look no more since ive already made one too [here](https://github.com/ICantMakeThings/StereoRecorder)

## Oshwa Certified!
<img width="100" height="160" alt="image" src="https://github.com/user-attachments/assets/cf43ca77-ceae-4da3-81a4-97c64b782f47" />

