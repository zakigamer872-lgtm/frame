#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QString>

class VideoEditor : public QWidget {
    Q_OBJECT
public:
    explicit VideoEditor(QWidget* parent = nullptr);

    void loadFile(const QString& filePath);

signals:
    void exportRequested(const QString& outputPath);

private slots:
    void onPlayPause();
    void onTrim();
    void onSplit();
    void onExport();
    void onSeek(int value);
    void onPlaybackTick();
    void onAddText();
    void onAddMusic();
    void onSpeedChange(double speed);

private:
    void setupUI();
    void setupTimeline();
    void setupToolbar();
    void setupPreview();
    void updatePlayButton();
    QString formatTime(qint64 ms) const;

    QString m_filePath;
    bool m_playing = false;
    qint64 m_duration = 0;
    qint64 m_position = 0;
    double m_playbackSpeed = 1.0;

    // UI
    QLabel* m_previewLabel;
    QSlider* m_timelineSlider;
    QLabel* m_positionLabel;
    QLabel* m_durationLabel;
    QPushButton* m_playBtn;
    QPushButton* m_trimBtn;
    QPushButton* m_splitBtn;
    QPushButton* m_exportBtn;
    QTimer* m_playTimer;
};
