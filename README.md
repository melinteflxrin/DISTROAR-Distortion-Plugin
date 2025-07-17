# DISTROAR

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Knobs](#knobs)
- [Download](#download)

## Description

**DISTROAR** is a gritty, heavy distortion plugin I made to get a punchy, aggressive sound from my electric guitar, perfect for chugs, riffs and leads. It's my first plugin, built in C++ with JUCE, and runs as a VST3 in any DAW.

## Features
- Multi-band distortion: splits the signal into low, mid, and high bands for separate processing
- Adaptive drive
- Multiple distortions: soft and hard clipping, custom wave shaping, dynamic smoothing
- Pre and post distortion compression for a consistent, level sound
- Cab sim: adds amp-like sound

## Knobs
- **Drive**: amount of distortion applied
- **Tone**: a simple low-pass filter
- **Blend**: mix between the clean and distorted signals
- **Output Gain**: adjust the final output level
- **Gate**: reduce noise by removing low-level signals before and after distortion

## Download
1. Download the latest `DISTROAR.vst3` or `DISTROAR.dll` file from [Releases](https://github.com/melinteflxrin/DISTROAR-Distortion-Plugin/releases)
2. Copy the file to your DAW's VST3 plugin folder:
   - **Windows:** `C:/Program Files/Common Files/VST3/`
   - **macOS:** `/Library/Audio/Plug-Ins/VST3/`
3. Open your DAW and rescan plugins