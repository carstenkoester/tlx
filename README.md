# tlx
This is the beginning of a a C++ library for a theatre-style lighting controller.

This is the beginning of a a C++ library for a theatre-style lighting controller. I started this library in
late 2013, with the aim to create a DMX lighting controller loosely based around the following principles:

- simple, theatre-style lighting (i.e. operating around a cue stack), as opposed to several very good
  alternatives that are already out there for concert lighting.
- lightweight and modular -- clear separation of backend and UI, the ability to run different UIs (text based, graphical,
  potentially brower based or CLI/API based) and a backend that is lightweight enough to run on a Raspberry Pi
- Use OpenDMX/OLA at the device layer

This project never completed, and what remains (so far) is a half-finished set of C++ files that could eventually become the backend library. In a nutshell, this code was planned to implement the following objects:

- Universe: A DMX universe (acting as an interface towards OLA). Each application has one or more DMX universes.
- Dimmer: A single DMX dimmer channel within a universe. Currently, only 8-bit channels are implemented.
- Channel: A single channel within the lighting application. As in most theatre lighting applications, a single channel can be patched to zero, one or more dimmers.
- Scene: A "snapshot" of one or more channels level. Note that in this implementation, a scene is really just that: An association of a specific channel to a level. In this implementation, scenes do not have any timers associated with them, nor are they in any particular sequence (see below for Cue objects). This is somewhat similar to what is called a "group" in some other lighting consoles.
- Cue: Essentially a wrapper around a Scene, adding timer values (fade-in/fade-out times).
- CueStack: An ordered list of cues.
- Crossfader: A device that iterates through a cuestack, either by hardware input or timer-driven.
- Submaster: A device that controls (fades in/out) a scene
