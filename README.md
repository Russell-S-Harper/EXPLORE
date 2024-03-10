# EXPLORE

This is a 3-D demo written in 1992 for DOS x86 computers being ported to the Commander X16.

## History

In chronological order:

- mid 1980s, met with a company in Chatsworth, CA to discuss a 3-D game for a popular 6502 system
- later 1980s, using every scrap of CPU and memory created a very simple demo in 6502 assembler; it was just walls that you could fly over and around, still looked cool
- slightly later 1980s, company goes bankrupt, project scrapped!
- early 1990s, rewrote the demo for the 68K in mixed K&R C and 68K assembler, with walls, polygons, horizon, sound, etc., but picked the "wrong horse" because the x86 was rapidly becoming dominant
- early 1990s, rewrote the demo for the x86 in DOS in mostly ANSI C, no sound (native sound was just beeps back then)
- mid 1990s, used the demo to get a number of decent summer jobs!
- late 1990s, graphics cards made 3-D development much easier and faster, stopped developing the demo (e.g. just had yaw, was planning on adding pitch and roll)
- 2000s to early 2020s, doing other things, forgot completely about the demo!
- 2024, viewed a [YouTube video](https://www.youtube.com/watch?v=t2ESLQHOIhw) from David Murray, The 8-Bit Guy, where he mentioned he was interested in seeing 3-D games added to the body of existing programs available for the Commander X16 (a modern 6502 computer, a.k.a. CX16); that reminded me of the old demo from last century :wink:
- 2024, found out the old demo can run in DOSBox!
- 2024 to present day, rewriting the demo for the CX16; the results so far are not impressive, but there are still opportunities to research

So from the mid 1980s to the present day, went from the 6502, to the 68K, then to the x86, then full circle back to the 6502!

## DOSBox

This is the original demo from 1992. I was still learning ANSI C at the time, so the code is not very good! As I port over functionality to the CX16 demo, I'm trying to clean it up and optimize it, before continuing with more functionality. Some places I'm really going to have to work to figure it out! Because the code was ported from a 68K base where it had a lot of registers available, the original code still has remnants of these keywords, as well as a lot of other strange ideas I had at the time.

To run the original demo, install [DOSBox](https://www.dosbox.com/), run it, mount a drive to point to `«explore-repo»/dosbox/bin`, and run `rgntest.exe`. Suggest running at least at 5 frames/s, i.e. press CTRL-F12 until it's fast enough. Here's a [video](https://www.youtube.com/watch?v=XTOIfkqW9O0) of it in action.

Controls:

- numeric pad 1 and 0: accelerate and deaccelerate
- cursor keys: climb, descend, turn left and right
- mouse: slide around
- left button mouse click: "target" players, probably had some ideas for this at some time
- right button mouse click: cycle to the next player (the two-triangle things, this was way before drones!)
- escape: pause, press again to resume
- CTRL-Q: quit, then displays a histogram<sup>†</sup> of how long it took to render all the frames

To build the original demo, you'll probably need Turbo C++. I distilled some original batch scripts into a sequence that might work as `«explore-repo»/dosbox/src/build.bat`, but no guarantees. It's not huge on my list of priorities!

† The histogram is in units of 13.7 ms (1/72.82 Hz), so to get 5 frames/s (or 0.20 s/frame), you'll want the peak at 15 units or less.

## CX16 – v1. Masking Approach

Completed the following:

- uses two 16-color screens at 320×240 resolution, displaying one while drawing on the other, then swapping
- uses an extended memory driver
- loads pre-calculated data into extended memory
- renders masked polygons

As compared to the original demo running in DOSBox, even with a very simplified image, it is very slow. It was originally taking 1.30 s/frame and was brought down to 0.66 s/frame but it's still far from the 0.20 s/frame goal. After optimizing and calculating durations, this is how long each task takes per frame in the very simplified image:

- `282 ms` – callback logic, for every point, a callback is made so it can check against the mask whether to plot the point; also polygons keep track of the left and right sides; the mask is very large, so it has to be maintained in extended memory, a lot of effort went into optimizing this
- ` 33 ms` – callback call overhead, this is the time spent just making the calls
- `282 ms` – drawing lines, these are the loops to draw lines using Bresenham’s Line Generation algorithm; I couldn't use the built-ins because: 16-color & 2 pixels/byte, and need to invoke a callback for each point
- ` 17 ms` – rotating the points, sin and cos are pre-calculated, so this saves a lot
- ` 50 ms` – baseline, these are routines to manage the dual screens: swapping and clearing, and clearing some of the mask

Suppose the complexity of the image is tripled, i.e. three horizon lines and six polygons, you can probably make something decent with that. The estimated timing would be:

- `845 ms` – callback logic
- `100 ms` – callback call overhead
- `847 ms` – drawing lines
- ` 50 ms` – rotating the points
- ` 50 ms` – baseline, unchanged

To achieve 5 frames/s, that corresponds to a budget of 0.20 s/frame or 200 ms/frame. However note that the just the *callback call overhead* + *rotating the points* + *baseline* already adds up to 200 ms! All of these have been significantly optimized, so even if everything was written in 6502 assembly, there would not be enough improvement expected.

### In summary, while rendering polygons using masking is an elegant solution and provides a lot of flexibility, it isn't appropriate for this platform!

Should you want to build the demo, you'll need [cc65](https://github.com/cc65/cc65), [FLT](https://github.com/Russell-S-Harper/FLT), and [x16emu](https://github.com/x16community/x16-emulator). Check these repositories if there are any other dependencies. Be sure to adhere to the licensing terms provided in these repositories to ensure proper usage and compliance.

Edit the `«flt-repo»/flt/build-cc65` script to point `XCC` to where the ***cc65*** repo is located, and run the script to build the `flt.lib` library.

Then edit the `«explore-repo»/cx16-v1/build-cc65-cx16` script to point `XCC`, `FLT`, and `EMU` to where the ***cc65***, ***FLT*** and ***x16emu*** repositories are located, and run the script to create `explore.prg` and `data.prg`.

Run the emulator, then load and run `data.prg` to generate the `explore.dat` data file, and then load and run `explore.prg` to run the demo. A word of warning, you'll likely be underwhelmed! Once `explore.dat` is created, you don't need to run `data.prg` again, unless you change `«explore-repo»/cx16-v1/data.c`.

The only control is Q to quit, or you can just wait an eternity for 100 frames to finish. Depending on what I'm optimizing, it might print some timing statistics in `clock` units.

Here's a stunning :wink: [video](https://www.youtube.com/watch?v=TsXz8cJG-AU) of the very simplified image running at 0.61 s/frame.

## CX16 – v2. Masking Approach

I'm currently researching if there are ways to leverage VERA, a display co-processor used in the CX16. VERA is currently being used to replicate some Doom-like games, so there might be a way to use it to draw filled polygons. There is also a new feature in VERA, "FX", which aims to improve polygon rendering. If I run out of options, might look into a hardware solution, like a cartridge with a co-processor on it.

## License

This repository is governed by a dual license – one for individual users and another for commercial users. Please review the respective licenses in the repository for complete details. For commercial distribution rights, a separate Commercial License is required, which can be obtained by contacting the repository owner russell.s.harper@gmail.com.

Your use of this code is subject to these license terms.

---

If you have any questions and/or suggestions, would like to discuss commercial distribution rights, or if you find EXPLORE useful for your project, we encourage you to reach out to:

Russell Harper

russell.s.harper@gmail.com

2024-03-06
