# EXPLORE

This is a 3-D demo written in 1992 for DOS computers being ported to CX16.

## History

In chronological order:

- mid 1980s, met with a company in Chatsworth, CA to discuss a 3-D game for a popular 6502 system
- later 1980s, using every scrap of CPU and memory created a very simple demo in 6502 assembler; it was just walls that you could fly over and around, still looked cool
- slightly later 1980s, company goes bankrupt, project scrapped!
- early 1990s, rewrote the demo for the 68K in mixed K&R C and 68K assembler, with polygons, sound, horizon, etc., but picked the "wrong horse" because the x86 was rapidly becoming dominant
- early 1990s, rewrote the demo for the x86 in DOS in mostly ANSI C, no sound (native sound was just beeps back then)
- later 1990s, use the demo to get a number of decent summer jobs!
- slightly later 1990s, graphics cards made 3-D development much easier and faster, stopped developing the demo (e.g. just had yaw, was planning on adding pitch and roll)
- 2000 to early 2020s, doing other things, forgot completely about the demo!
- 2024, viewed a YouTube [video](https://www.youtube.com/watch?v=t2ESLQHOIhw) from David Murray, The 8-Bit Guy, where he mentioned he was interested in seeing 3-D games added to the body of existing programs available for the CX16 (a modern 6502 computer); that reminded me of the old demo from last century :wink:
- 2024, found out the old demo can run in DOSBox!
- 2024 to present day, rewriting the demo for the CX16; the results so far are not impressive, but there are still opportunities to research

So from the mid 1980s to the present day, went from the 6502, to the 68K, then to the x86, then full circle back to the 6502!

## DOSBox

This is the original x86 demo from 1992. I was still learning ANSI C at the time, so the code is not very good! As I port over functionality to the CX16 demo, I'm trying to clean it up and optimize it, before continuing with more functionality. Some places I'm really going to have to work to figure it out! Because the code was ported from a 68K base where it had a lot of registers available, the DOSBox code still has remnants of these, as well as a lot of other strange ideas I had at the time.

To run the DOSBox demo, install [DOSBox](https://www.dosbox.com/), run it, mount a drive to point to `«explore-repo»/dosbox/bin`, and run `rgntest.exe`. Suggest running at least at 5 frames/s, i.e. press CTRL-F12 until it's fast enough. Here's a [video](https://youtu.be/XTOIfkqW9O0?si=wh1oJeDKlLNBwxkr) of it in action.

Controls:

- numeric pad 1 and 0: speed up and slow down
- cursor keys: climb, descend, turn left and right
- mouse: slide around
- left mouse click: "target" players, probably had some ideas for this at some time
- right mouse click: cycle to the next player (the two-triangle things, this was way before drones!)
- escape: pause, press again to resume
- CTRL-Q: quit, then displays a histogram<sup>†</sup> of how long it took to render all the frames

To build the DOSBox demo, you'll probably need Turbo C++. I distilled some original batch scripts into a sequence that might work as `«explore-repo»/dosbox/src/build.bat`, but no guarantees. It's not huge on my list of priorities!

† The histogram is in units of 13.7 ms (1/72.82 Hz), so to get 5 frames/s (or 0.20 s/frame), you'll want the peak at 15 units or less.

## CX16

This is a work in progress. So far:

- uses two 16-color screens at 320x240 resolution, displaying one while drawing on the other, then swapping
- uses an extended memory driver
- loads pre-calculated data into extended memory
- renders masked polygons

As compared to the DOSBox "Explore" demo, even with a very simplified image, it is very slow at 0.91 s/frame, far from the 0.20 s/frame goal. I'm currently researching if there are ways to leverage VERA, a display co-processor used in the CX16. VERA is currently being used to replicate some Doom-like games, so there might be a way to use it to draw filled polygons. If I run out of options, might look into a hardware solution, like a cartridge with a co-processor on it.

To build the demo, you'll need [cc65](https://github.com/cc65/cc65), [FLT](https://github.com/Russell-S-Harper/FLT), and [x16emu](https://github.com/x16community/x16-emulator). Check these repositories if there are any other dependencies. Be sure to adhere to the licensing terms provided in these repositories to ensure proper usage and compliance.

Edit the `«flt-repo»/flt/build-cc65` script to point `XCC` to where the ***cc65*** repo is located, and run the script to build the `flt.lib` library.

Then edit the `«explore-repo»/cx16/build-cc65-cx16` script to point `XCC`, `FLT`, and `EMU` to where the ***cc65***, ***FLT*** and ***x16emu*** repositories are located, and run the script to create `explore.prg` and `data.prg`.

Run the emulator, then load and run `data.prg` to generate the `explore.dat` data file, and then load and run `explore.prg` to run the demo. A word of warning, you'll likely be underwhelmed! Once `explore.dat` is created, you don't need to run `data.prg` again, unless you change `«explore-repo»/cx16/data.c`.

The only control is Q to quit, or you can just wait an eternity for 100 frames to finish. Depending on what I'm optimizing, it might print some timing statistics in `clock` units.

Here's a stunning :wink: [video](https://www.youtube.com/watch?v=H0h6VMAwbqY) of the progress to date.

## License

This repository is governed by a dual license – one for individual users and another for commercial users. Please review the respective licenses in the repository for complete details. For commercial distribution rights, a separate Commercial License is required, which can be obtained by contacting the repository owner russell.s.harper@gmail.com.

Your use of this code is subject to these license terms.

---

If you have any questions and/or suggestions, would like to discuss commercial distribution rights, or if you find EXPLORE useful for your project, we encourage you to reach out to:

Russell Harper

russell.s.harper@gmail.com

2024-03-06
