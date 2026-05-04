#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

class AudioCapture : public QObject {
    Q_OBJECT
public:
    explicit AudioCapture(QObject* parent = nullptr);
    ~AudioCapture();

    void setSystemAudio(bool enabled)       { m_systemAudio = enabled; }
    void setMicrophone(bool enabled)        { m_microphone = enabled; }
    void setMicDevice(const QString& dev)   { m_micDevice = dev; }
    void setNoiseSuppression(bool enabled)  { m_noiseSuppression = enabled; }
    void setEchoCancellation(bool enabled)  { m_echoCancellation = enabled; }
    void setSystemVolume(int vol)           { m_systemVolume = qBound(0, vol, 100); }
    void setMicVolume(int vol)              { m_micVolume = qBound(0, vol, 100); }

    void start();
    void stop();
    void pause();
    void resume();

    QStringList availableDevices() const;
    float currentSystemLevel() const { return m_systemLevel; }
    float currentMicLevel()    const { return m_micLevel; }
    bool isRunning()           const { return m_running; }

signals:
    void levelChanged(float systemLevel, float micLevel);
    void error(const QString& msg);

private:
    bool    m_systemAudio      = true;
    bool    m_microphone       = true;
    bool    m_noiseSuppression = false;
    bool    m_echoCancellation = false;
    int     m_systemVolume     = 100;
    int     m_micVolume        = 100;
    QString m_micDevice;
    bool    m_running          = false;
    float   m_systemLevel      = 0.0f;
    float   m_micLevel         = 0.0f;

    QTimer* m_levelTimer = nullptr;
};
