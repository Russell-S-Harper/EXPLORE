# EXPLORE

These are some demos written in the 1990s for DOS x86 and 8-bit computers being ported to the Commander X16. One is a 3-D demo; the other a very basic AI demo.

The 3-D demo was written in 1992 for DOS x86 computers for which I still have the original source code. The other AI demo is a game long lost to history – the only things left are a few hand-drawn sketches and flowcharts.

## DOSBox

This is the original 3-D demo from 1992. I was still learning ANSI C at the time, so the code is not very good! As I ported over functionality to the CX16 demo, I tried to clean it up and optimize it, before continuing with more functionality. Some places I really had to work hard to figure it out! Because the code was ported from a 68K base where it had a lot of registers available, the original code still has remnants of these keywords, as well as a lot of other strange ideas I had at the time.

To run the original demo, install [DOSBox](https://www.dosbox.com/), run it, mount a drive to point to `«explore-repo»/dosbox/bin`, and run `rgntest.exe`. Suggest running at least at 5 frames/s, i.e. press CTRL-F12 until it’s fast enough. Here’s a [video](https://www.youtube.com/watch?v=XTOIfkqW9O0) of it in action.

Controls:

- numeric pad 1 and 0: accelerate and deaccelerate
- cursor keys: climb, descend, turn left and right
- mouse: slide around
- left button mouse click: "target" players, probably had some ideas for this at some time
- right button mouse click: cycle to the next player (the two-triangle things, this was way before drones!)
- escape: pause, press again to resume
- CTRL-Q: quit, then displays a histogram<sup>†</sup> of how long it took to render all the frames

To build the original demo, you’ll probably need Turbo C++. I distilled some original batch scripts into a sequence that might work as `«explore-repo»/dosbox/src/build.bat`, but no guarantees. It’s not huge on my list of priorities!

† The histogram is in units of 13.7 ms (1/72.82 Hz), so to get 5 frames/s (or 0.20 s/frame), you’ll want the peak at 15 units or less.

## CX16 v1 – 3-D Demo

### History

- mid 1980s, met with a company in Chatsworth, CA to discuss a 3-D game for a popular 6502 system
- later 1980s, using every scrap of CPU and memory created a very simple demo in 6502 assembler; it was just walls that you could fly over and around, still looked cool
- slightly later 1980s, company goes bankrupt, project scrapped!
- early 1990s, rewrote the demo for the 68K in mixed K&R C and 68K assembler, with walls, polygons, horizon, sound, etc., but picked the "wrong horse" because the x86 was rapidly becoming dominant
- early 1990s, rewrote the demo for the x86 in DOS in mostly ANSI C, no sound (native sound was just beeps back then)
- mid 1990s, used the demo to get a number of decent summer jobs!
- late 1990s, graphics cards made 3-D development much easier and faster, stopped developing the demo (e.g. just had yaw, was planning on adding pitch and roll)
- 2000s to early 2020s, doing other things, forgot completely about the demo!
- 2024, viewed a [YouTube video](https://www.youtube.com/watch?v=t2ESLQHOIhw) from David Murray, The 8-Bit Guy, where he mentioned he was interested in seeing 3-D games added to the body of existing programs available for the Commander X16 (a modern 6502 computer, a.k.a. CX16); that reminded me of the old demo from last century :wink:
- 2024, found out the demo can still run in DOSBox!
- 2024 to present day, rewriting the demo for the CX16; so far the results are not impressive – project on hold

So from the mid 1980s to the present day, went from the 6502, to the 68K, then to the x86, then full circle back to the 6502!

### Progress

Completed the following:

- uses two 16-color screens at 320×240 resolution, displaying one while drawing on the other, then swapping
- uses an extended memory driver
- loads pre-calculated data into extended memory
- renders masked polygons

As compared to the original demo running in DOSBox, even a very simplified demo is very slow. It was originally taking 1.30 s/frame and was brought down to 0.66 s/frame but it’s still far from the 0.20 s/frame goal. After optimizing and calculating durations, this is how long each task takes per frame in the very simplified demo:

- `282 ms` – callback logic, for every point, a callback is made so it can check against the mask whether to plot the point; also polygons keep track of the left and right sides; the mask is very large, so it has to be maintained in extended memory, a lot of effort went into optimizing this
- ` 33 ms` – callback call overhead, this is the time spent just making the calls
- `282 ms` – drawing lines, these are the loops to draw lines using Bresenham’s Line Generation algorithm; I couldn’t use the built-ins because: 16-color & 2 pixels/byte, and need to invoke a callback for each point
- ` 17 ms` – rotating the points, sines and cosines are pre-calculated, so this saves a lot
- ` 50 ms` – baseline, these are routines to manage the dual screens: swapping and clearing, and clearing some of the mask

Suppose the complexity of the image is tripled, i.e. three horizon lines and six polygons, you can probably make a decent game with that. The estimated timing would be:

- `845 ms` – callback logic
- `100 ms` – callback call overhead
- `847 ms` – drawing lines
- ` 50 ms` – rotating the points
- ` 50 ms` – baseline, unchanged

To achieve 5 frames/s, that corresponds to a budget of 0.20 s/frame or 200 ms/frame. However note that just the *callback call overhead* + *rotating the points* + *baseline* already add up to 200 ms! All of these have been significantly optimized, so even if everything was written in 6502 assembly, there would not be enough improvement expected.

### In summary, while rendering polygons using masking is an elegant solution and provides a lot of flexibility, it isn’t appropriate for this platform!

Should you want to build the demo, you’ll need [cc65](https://github.com/cc65/cc65), [FLT](https://github.com/Russell-S-Harper/FLT), and [x16emu](https://github.com/x16community/x16-emulator). Check these repositories if there are any other dependencies. Be sure to adhere to the licensing terms provided in these repositories to ensure proper usage and compliance.

Edit the `«flt-repo»/flt/build-cc65` script to point `XCC` to where the ***cc65*** repo is located, and run the script to build the `flt.lib` library.

Then edit the `«explore-repo»/cx16-v1/build-cc65-cx16` script to point `XCC`, `FLT`, and `EMU` to where the ***cc65***, ***FLT*** and ***x16emu*** repositories are located, and run the script to create `EXPLORE.PRG` and `DATA.PRG`.

Run the emulator, then load and run `DATA.PRG` to generate the `EXPLORE.DAT` data file, and then load and run `EXPLORE.PRG` to run the demo. A word of warning, you’ll likely be underwhelmed! Once `EXPLORE.DAT` is created, you don’t need to run `DATA.PRG` again, unless you change `«explore-repo»/cx16-v1/data.c`.

The only controls are escape to pause (!) and Q to quit, or you can just wait an eternity for 100 frames to finish. Depending on what I’m optimizing, it might print some timing statistics in `clock` units.

Here’s a stunning :wink: [video](https://www.youtube.com/watch?v=TsXz8cJG-AU) of the very simplified demo running at 0.66 s/frame.

## CX16 v2 – AI Demo (a.k.a. PvP-AI)

### History

- early 1990s, wrote an AI demo set in an arena with competing players, however most details are lost to history
- 2024 to present day, rewriting the demo for the CX16; so far the results are good – might have something useful for exhibitions and trade shows

### Progress

VERA is a display co-processor used in the CX16. It has a new feature, "FX", which provides helpers to improve line drawing, polygon filling, and other functions. Using the line drawing helper, I was able to implement 16-color line drawing routines such that, even written in C and implementing clipping, they still run in about 75% of the time required by the 256-color line drawing routines in TGI. As an aside, the line drawing routines in `cx16-v1` using Bresenham’s algorithm took 3× the time to run than the TGI drawing routines, so a huge improvement gain using VERA.

V2 takes a different perspective looking overhead into an arena setting with walls and vehicles. There’s no masking, just lines. Here’s a [video](https://www.youtube.com/watch?v=LiRbyPf7lw4) of the current progress. The rotations and scalings require a lot of multiplications and divisions such that they end up being bottlenecks. The optimizer in cc65 and the multiplier & line drawing helper in VERA are major factors in getting the frame rate to 8.6 frames/s, with enough unused time between frames to give a decent timeslice for the AI.

For those who want to try out the compiled program, you can copy the three files beginning with `PVP-AI` from my [Google Drive](https://drive.google.com/drive/folders/1nBoKQ6ZB6Fb3NYO77jeoF8aATl1pe7U-?usp=sharing) and run the `PRG`. Suggest running in an emulator (R48) for now because there is a problem with the VERA hardware, see the note below. As well, there might be issues with handling files in hardware using the `stdio` functions – still investigating.

About the AI, it uses only 13 bytes to correspond to various settings, e.g. evade, revenge, persistence, etc. You can let the program run on its own, and it will create an `AI.DAT` file and document the generations as the players compete with each other. It has an attract sequence and a summary screen and looks pretty good running as a screen saver or a retro videogame attract mode.

As in `cx16-v1`, should you want to build the demo, you’ll need [cc65](https://github.com/cc65/cc65), [FLT](https://github.com/Russell-S-Harper/FLT), and [x16emu](https://github.com/x16community/x16-emulator). Check these repositories if there are any other dependencies. Be sure to adhere to the licensing terms provided in these repositories to ensure proper usage and compliance.

Edit the `«flt-repo»/flt/build-cc65` script to point `XCC` to where the ***cc65*** repo is located, and run the script to build the `flt.lib` library.

Then edit the `«explore-repo»/cx16-v2/build-cc65-cx16` script to point `XCC`, `FLT`, and `EMU` to where the ***cc65***, ***FLT*** and ***x16emu*** repositories are located, and run the script to create `EXPLORE.PRG`, `TEXT.PRG`, and `DATA.PRG`.

Run the emulator, then load and run `TEXT.PRG` & `DATA.PRG` to generate the `PVP-AI.TXT` & `PVP-AI.DAT` text and data files (you might want to run these with `-warp` specified as in `«explore-repo»/cx16-v2/build-cc65-cx16`), and then load & run `PVP-AI.PRG` to run the demo. Once the text and data files are created, you don’t need to run `TEXT.PRG` & `DATA.PRG` again, unless you change `«explore-repo»/cx16-v2/data.c` or `«explore-repo»/cx16-v2/text.c`.

Note: The hardware currently has a problem in that in 16-color mode, VERA can’t render some lines at certain angles. As a workaround these lines are handled by implementing Bresenham’s algorithm directly, but the end result is that in hardware it runs at only 4.0 frames/s – I think too slow to be playable.

The demo uses these keyboard controls:

- escape: pause, press again to resume
- C: cycle player
- Q: quit
- J: join the game in progress, your vehicle will have a little "cockpit"
- K: join the game, but you’re the preferred target (hard)
- L: join the game, but for you the last level doesn’t restore its health after zeroing other players (very hard)

When you’re playing, these are your keyboard and gamepad controls:

- cursor up or gamepad back: climb
- cursor down or gamepad forward: dive
- cursor right or gamepad right: turn right
- cursor left or gamepad left: turn left
- space or gamepad button: fire missile

## License

This repository is governed by a dual license – one for individual users and another for commercial users. Please review the respective licenses in the repository for complete details. For commercial distribution rights, a separate Commercial License is required, which can be obtained by contacting the repository owner russell.s.harper@gmail.com.

Your use of this code is subject to these license terms.

---

If you have any questions and/or suggestions, would like to discuss commercial distribution rights, or if you find EXPLORE useful for your project, feel free to contact:

Russell Harper

russell.s.harper@gmail.com

2024-03-06
