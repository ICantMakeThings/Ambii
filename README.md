## What is Ambii?
Its short for [Ambisonics](https://en.wikipedia.org/wiki/Ambisonics) Which is a [Binaural Microphone](https://en.wikipedia.org/wiki/Binaural_recording) on sterioids- Kind of...

This repo includes the code for the recorder, since I like to make stuff as cheap as I can, and quad input recording devices are mad expensive, and ohh you want to include portibility in the search? haa.. ha.

this project includes everything, including the 3d printable files to print the actual mic too. once im happy enough with the files they will be on [thingiverse](https://www.thingiverse.com/cantmakethings)

okay okay enough, components,

## Required Components

- **ESP32-S3 Mini**: [AliExpress Link](https://aliexpress.com/item/1005005449219195.html)
- **PCM1808 (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_oCrSlXR)
- **Aux (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_o2d4zcZ)
- **Microphone Capsule (need 4pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_oltEmLb)
- **SD Card**: [AliExpress Link](https://s.click.aliexpress.com/e/_oFsbpBB)
- **SD Card Holder**: [AliExpress Link](https://s.click.aliexpress.com/e/_olg0Y8z)
- **OLED Display**: [AliExpress Link](https://s.click.aliexpress.com/e/_olLPabx)
- **Mic Preamp (need 2pcs)**: [AliExpress Link](https://s.click.aliexpress.com/e/_o2CUG6t)
- Some thin wires to connect the parts together

*Note: Some of thease are referral links. If you purchase through it, I earn a commission at no extra cost to you.*

<img width="1819" height="991" alt="image" src="https://github.com/user-attachments/assets/71df9c74-bb81-427d-ac3e-1ceb55fa1cc0" />
<img width="1707" height="995" alt="image" src="https://github.com/user-attachments/assets/417cd24c-3441-4a24-96cc-81c0b38045fd" />

The Wiring is done in such a way where most of the pins are soldered with pins with a little gap:

<img width="1094" height="827" alt="image" src="https://github.com/user-attachments/assets/e456e050-0f12-46bd-a41e-4a657a8f8936" />

I soldered the pins to the PCM, then removed the plastic spacers and soldered the S3 on top, 

Only BCK has a wire to a gpio since it didnt fit, on both PCM's you want to short PMT, MD1, MD0 with GND, 3v to 3v and 5v to 5v. 

As of rn, the Mic Preamps havent arrived, I assume they work but we will see, 

It records *24-bit PCM @ 48 kHz* With a possibility to bump that up to 96 kHz, but dont think I will.


###### Only interested in 2 channels? look no more since ive already made one too [here](https://github.com/ICantMakeThings/StereoRecorder)

