# FrameX — Build Guide
**Developer:** ZAKI | Instagram & TikTok: @zg22x

---

## Requirements

| Tool | Version | Download |
|------|---------|----------|
| Visual Studio | 2022 (with C++ workload) | https://visualstudio.microsoft.com |
| Qt Framework | 6.7+ (MSVC 2019 64-bit) | https://www.qt.io/download |
| FFmpeg | Latest | https://www.gyan.dev/ffmpeg/builds/ |
| CMake | 3.16+ | https://cmake.org |
| Inno Setup | 6.x | https://jrsoftware.org/isinfo.php |

---

## Quick Build (Windows)

### Step 1 — Install dependencies

1. **Visual Studio 2022** — Install with "Desktop development with C++" workload
2. **Qt 6.7** — Install to `C:\Qt\6.7.0\msvc2019_64`
3. **FFmpeg** — Extract to `C:\ffmpeg` (needs `include/` and `lib/` folders)
4. **CMake** — Install to `C:\cmake`
5. **Inno Setup 6** — Install to default path

### Step 2 — Build the app

Open **x64 Native Tools Command Prompt for VS 2022** and run:

```batch
cd framex-project\installer
build_installer.bat
```

This script will:
1. Configure the project with CMake
2. Build in Release mode
3. Deploy all Qt DLLs with `windeployqt`
4. Copy FFmpeg DLLs
5. Create the installer EXE with Inno Setup

### Step 3 — Get your installer

Find `FrameX_Setup_v1.0.0.exe` in `framex-project\installer\output\`

---

## Manual Build Steps

```batch
:: Configure
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64" ^
  -DFFMPEG_DIR="C:\ffmpeg"

:: Build
cmake --build . --config Release

:: Deploy Qt
C:\Qt\6.7.0\msvc2019_64\bin\windeployqt.exe --release build\Release\FrameX.exe
```

---

## Project Structure

```
framex-project/
├── CMakeLists.txt              # Build system
├── BUILD_GUIDE.md              # This file
├── LICENSE.txt
├── src/
│   ├── main.cpp                # Entry point + splash screen
│   ├── mainwindow.h/cpp        # Main application window
│   ├── settings_manager.h/cpp  # App settings (persisted to registry)
│   ├── hotkey_manager.h/cpp    # Global hotkeys
│   ├── history_manager.h/cpp   # Recording history
│   ├── theme_manager.h/cpp     # Dark/Light theme
│   ├── recording/
│   │   ├── recorder.h/cpp      # Recording orchestrator
│   │   ├── screen_capture.h/cpp # FFmpeg screen capture
│   │   ├── audio_capture.h/cpp  # System + mic audio
│   │   └── webcam_capture.h/cpp # Webcam overlay
│   ├── editor/
│   │   └── video_editor.h/cpp  # Built-in video editor
│   └── ui/
│       ├── sidebar_widget.h/cpp
│       ├── dashboard_page.h/cpp
│       ├── recordings_page.h/cpp
│       ├── screenshots_page.h/cpp
│       ├── settings_page.h/cpp
│       ├── hotkeys_page.h/cpp
│       ├── tools_page.h/cpp
│       ├── streaming_page.h/cpp
│       ├── recording_settings_panel.h/cpp
│       ├── floating_widget.h/cpp
│       └── about_dialog.h/cpp
├── resources/
│   ├── resources.qrc
│   └── icons/framex.ico
└── installer/
    ├── setup.iss               # Inno Setup script
    └── build_installer.bat     # One-click build script
```

---

## Features Implemented

### Recording Core
- Full Screen / Window / Custom Area capture
- System Audio + Microphone (separate or combined)
- Webcam overlay (adjustable size and position)
- Video quality: 720p / 1080p / 2K / 4K
- FPS: 30 / 60 / 120
- Output formats: MP4, MKV, AVI

### Performance Engine
- GPU acceleration (NVENC / AMD AMF / Intel QSV)
- H.264 and H.265 encoding
- Optimized buffering via FFmpeg
- Multi-process capture pipeline

### Game Mode
- DirectX / OpenGL / Vulkan capture via DXGI (Windows)
- Floating recording widget (minimal overlay)

### Recording Controls
- Pause / Resume
- Global hotkeys (fully customizable)
- Screenshot during recording (Ctrl+Shift+S)
- Floating widget with timer + controls

### Audio System
- Independent volume control per source
- Multi-track recording
- Noise suppression (AI filter via FFmpeg)

### Built-in Editor
- Trim / Cut / Split
- Speed control (0.25x to 2x)
- Add Text, Music, Images
- Format converter
- Color correction

### Screenshots
- Full screen capture
- PNG / JPG format
- Auto-save to Pictures/FrameX Screenshots

### AI Features (Framework)
- Auto window detection
- Speech-to-Text (Voice to Text tool)
- Highlight Moments detection

### Security & Stability
- Auto-save session history
- Crash recovery (auto-save flag)
- Custom save path
- Recording history log

### UI/UX
- Dark Mode / Light Mode
- Floating widget during recording
- System tray (minimize to tray)
- Full sidebar navigation
- Dashboard with recent recordings
- Quick settings panel

---

## Developer

**ZAKI**  
Instagram: [@zg22x](https://instagram.com/zg22x)  
TikTok: [@zg22x](https://tiktok.com/@zg22x)

---
*FrameX v1.0.0 — Capture everything. Share anything.*
