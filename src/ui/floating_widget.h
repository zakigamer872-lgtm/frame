#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

class FloatingWidget : public QWidget {
    Q_OBJECT
public:
    explicit FloatingWidget(QWidget* parent = nullptr);

    void startTimer();
    void stopTimer();
    void updateTime(qint64 secs);
    void setPaused(bool paused);

signals:
    void stopRequested();
    void pauseRequested();
    void screenshotRequested();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    QLabel* m_timeLabel;
    QPushButton* m_stopBtn;
    QPushButton* m_pauseBtn;
    QPushButton* m_ssBtn;
    bool m_dragging = false;
    QPoint m_dragPos;
    bool m_paused = false;
    QTimer* m_blinkTimer;
    bool m_blink = true;
};
