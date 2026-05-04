#include "webcam_capture.h"
#include <QPainter>
#include <QRandomGenerator>

WebcamCapture::WebcamCapture(QObject* parent) : QObject(parent) {
    m_frameTimer = new QTimer(this);
    m_frameTimer->setInterval(33); // ~30fps
    connect(m_frameTimer, &QTimer::timeout, [this]() {
        // In real implementation, grab from camera device
        // For now, create a placeholder frame
        QImage img(320, 240, QImage::Format_RGB888);
        img.fill(QColor(20, 20, 40));
        QPainter p(&img);
        p.setPen(QColor(160, 80, 255));
        p.drawText(img.rect(), Qt::AlignCenter, "Camera Feed");
        p.end();
        m_currentFrame = img;
        emit frameReady(img);
    });
}

WebcamCapture::~WebcamCapture() { stop(); }

void WebcamCapture::start() {
    m_running = true;
    m_frameTimer->start();
}

void WebcamCapture::stop() {
    m_running = false;
    m_frameTimer->stop();
}

QStringList WebcamCapture::availableDevices() const {
    return {"Logitech C922", "Logitech C920", "Built-in Camera", "USB Webcam"};
}
