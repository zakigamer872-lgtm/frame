#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QProgressBar>

class SidebarWidget : public QWidget {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget* parent = nullptr);

    void setRecordingState(bool recording);
    void updateStorageInfo(qint64 used, qint64 total);

signals:
    void pageSelected(int index);
    void recordingRequested();
    void themeToggleRequested();

private:
    void setupUI();
    QPushButton* makeNavButton(const QString& icon, const QString& text, int index);

    QButtonGroup* m_btnGroup;
    QLabel* m_storageUsedLabel;
    QProgressBar* m_storageBar;
    QLabel* m_userLabel;
    QLabel* m_recordingIndicator;
    bool m_isRecording = false;
};
