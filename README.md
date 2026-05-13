# 🎵 JUCE Audio Player

A professional dual-deck audio player built in C++ using the JUCE framework — featuring crossfade, playlist management, and real-time ID3 metadata display.

---

## ✨ Features

- 🎚️ **Dual-deck architecture** — two independent players running simultaneously via `MixerAudioSource`
- 🔀 **Crossfade / Mix Mode** — preloads next track and smoothly blends gain over a 15-second fade window
- 📋 **Playlist manager** — shuffle, A→B loop points, auto-advance, and ±10s skip controls
- 🏷️ **ID3 metadata** — real-time display of title, artist, and album via TagLib integration
- 🧱 **Clean OOP architecture** — audio engine (`PlayerAudio`) fully separated from UI layer (`PlayerGUI`)

---

## 🛠️ Tech Stack

| Component | Technology |
|---|---|
| Language | C++ |
| Audio Framework | JUCE |
| Metadata | TagLib |
| Build System | CMake / Projucer |

---

## 🏗️ Architecture

```
AudioPlayer/
├── PlayerAudio.h / .cpp    # Audio engine — playback, mixing, crossfade logic
├── PlayerGUI.h / .cpp      # UI layer — controls, waveform, metadata display
├── PlaylistManager.h/.cpp  # Shuffle, loop points, auto-advance
└── Main.cpp                # Entry point
```

**Design principle:** The audio engine and UI are completely decoupled. `PlayerAudio` knows nothing about the interface — it only exposes an API that `PlayerGUI` calls. This makes the codebase easy to test and extend.

---

## 🎛️ Key Implementation Details

**Crossfade logic:**
- When Mix Mode is enabled, the next track is preloaded silently
- As the current track approaches its end, gain is gradually shifted from deck A to deck B over 15 seconds
- Uses `MixerAudioSource` to blend two `AudioTransportSource` instances

**Loop points (A→B):**
- User sets an in-point (A) and out-point (B) during playback
- Player automatically loops between these positions until disabled

---

## 🚀 Getting Started

### Requirements
- JUCE framework (download from [juce.com](https://juce.com))
- TagLib
- C++17 compatible compiler

### Build
```bash
# Open in Projucer, configure exporter for your platform, then build
# Or use CMake:
mkdir build && cd build
cmake .. && make
```

---

*Built as a personal project — Cairo University, 2026*
