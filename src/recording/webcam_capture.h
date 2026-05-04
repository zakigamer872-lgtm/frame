#pragma once
#include <QObject>
#include <QString>
#include <QImage>
#include <QStringList>
#include <QTimer>

class WebcamCapture : public QObject {
    Q_OBJECT
public:
    explicit WebcamCapture(QObject* parent = nullptr);
    ~WebcamCapture();

    void setDevice(const QString& device)  { m_device = device; }
    void setSize(int percent)              { m_sizePercent = qBound(10, percent, 50); }
    void setPosition(const QString& pos)   { m_position = pos; }
    void setShape(const QString& shape)    { m_shape = shape; }

    void start();
    void stop();

    QStringList availableDevices() const;
    QImage currentFrame() const { return m_currentFrame; }
    bool isRunning() const { return m_running; }

signals:
    void frameReady(const QImage& frame);
    void error(const QString& msg);

private:
    QString m_device;
    int     m_sizePercent = 25;
    QString m_position    = "bottom-right";
    QString m_shape       = "circle";
    bool    m_running     = false;
    QImage  m_currentFrame;

    QTimer* m_frameTimer = nullptr;
};
