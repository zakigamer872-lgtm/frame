#include "floating_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

FloatingWidget::FloatingWidget(QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(200, 58);

    // Position bottom center
    QScreen* screen = QApplication::primaryScreen();
    QRect sg = screen->availableGeometry();
    move((sg.width() - width()) / 2, sg.height() - height() - 50);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->setSpacing(8);

    // Recording indicator dot
    auto* dotLabel = new QLabel("●");
    dotLabel->setStyleSheet("color: #EF4444; font-size: 14px;");
    layout->addWidget(dotLabel);

    // Time
    m_timeLabel = new QLabel("00:00:00");
    m_timeLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    layout->addWidget(m_timeLabel);
    layout->addStretch();

    // Pause
    m_pauseBtn = new QPushButton("⏸");
    m_pauseBtn->setFixedSize(28, 28);
    m_pauseBtn->setStyleSheet("QPushButton { background: rgba(255,255,255,0.15); border: none; border-radius: 6px; color: white; font-size: 12px; }"
                              "QPushButton:hover { background: rgba(255,255,255,0.3); }");
    connect(m_pauseBtn, &QPushButton::clicked, this, &FloatingWidget::pauseRequested);
    layout->addWidget(m_pauseBtn);

    // Screenshot
    m_ssBtn = new QPushButton("📷");
    m_ssBtn->setFixedSize(28, 28);
    m_ssBtn->setStyleSheet("QPushButton { background: rgba(255,255,255,0.15); border: none; border-radius: 6px; color: white; font-size: 11px; }"
                           "QPushButton:hover { background: rgba(255,255,255,0.3); }");
    connect(m_ssBtn, &QPushButton::clicked, this, &FloatingWidget::screenshotRequested);
    layout->addWidget(m_ssBtn);

    // Stop
    m_stopBtn = new QPushButton("■");
    m_stopBtn->setFixedSize(28, 28);
    m_stopBtn->setStyleSheet("QPushButton { background: #EF4444; border: none; border-radius: 6px; color: white; font-size: 12px; font-weight: bold; }"
                             "QPushButton:hover { background: #DC2626; }");
    connect(m_stopBtn, &QPushButton::clicked, this, &FloatingWidget::stopRequested);
    layout->addWidget(m_stopBtn);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);

    m_blinkTimer = new QTimer(this);
    m_blinkTimer->setInterval(800);
    connect(m_blinkTimer, &QTimer::timeout, [this, dotLabel]() {
        m_blink = !m_blink;
        dotLabel->setStyleSheet(QString("color: %1; font-size: 14px;")
            .arg(m_blink ? "#EF4444" : "transparent"));
    });
}

void FloatingWidget::startTimer() { m_blinkTimer->start(); }
void FloatingWidget::stopTimer() { m_blinkTimer->stop(); }

void FloatingWidget::updateTime(qint64 secs) {
    int h = secs / 3600, m = (secs % 3600) / 60, s = secs % 60;
    m_timeLabel->setText(QString("%1:%2:%3")
        .arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')));
}

void FloatingWidget::setPaused(bool paused) {
    m_paused = paused;
    m_pauseBtn->setText(paused ? "▶" : "⏸");
    if (paused) m_blinkTimer->stop();
    else m_blinkTimer->start();
}

void FloatingWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(15, 15, 31, 220));
    p.setPen(QColor(239, 68, 68, 100));
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 12, 12);
}

void FloatingWidget::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) { m_dragging = true; m_dragPos = e->globalPosition().toPoint() - pos(); }
}
void FloatingWidget::mouseMoveEvent(QMouseEvent* e) {
    if (m_dragging) move(e->globalPosition().toPoint() - m_dragPos);
}
void FloatingWidget::mouseReleaseEvent(QMouseEvent*) { m_dragging = false; }
