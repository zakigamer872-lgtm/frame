#pragma once
#include <QSettings>
#include <QString>
#include <QKeySequence>

struct RecordingSettings {
    QString captureMode       = "fullscreen";
    QString videoQuality      = "1080p";
    int     fps               = 60;
    QString encoder           = "H.264 (CPU)";
    QString outputFormat      = "MP4";
    QString savePath;
    bool    systemAudio       = true;
    bool    microphone        = true;
    bool    webcamEnabled     = false;
    QString webcamDevice;
    int     webcamSize        = 25;
    QString webcamPosition    = "bottom-right";
    bool    showCursor        = true;
    bool    highlightCursor   = false;
    bool    showClicks        = false;
    bool    gpuAcceleration   = false;
    int     audioBitrate      = 192;
    int     videoBitrate      = 8000;
};

struct HotkeySettings {
    QKeySequence startStop        = QKeySequence("Ctrl+Shift+R");
    QKeySequence pauseResume      = QKeySequence("Ctrl+Shift+P");
    QKeySequence screenshot       = QKeySequence("Ctrl+Shift+S");
    QKeySequence cancelRecording  = QKeySequence("Ctrl+Shift+X");
    QKeySequence openEditor       = QKeySequence("Ctrl+Shift+E");
};

struct AppSettings {
    QString theme               = "dark";
    bool    minimizeToTray      = true;
    bool    startOnBoot         = false;
    bool    showFloatingWidget  = true;
    bool    hardwareAcceleration = true;
    bool    showNotifications   = true;
    QString language            = "en";
    int     autoSplitMinutes    = 0;
};

class SettingsManager {
public:
    static SettingsManager& instance() {
        static SettingsManager inst;
        return inst;
    }

    void initialize();
    void save();
    void load();
    void resetToDefaults();

    RecordingSettings recording;
    HotkeySettings    hotkeys;
    AppSettings       app;

private:
    SettingsManager() = default;
    QSettings* m_settings = nullptr;
};
