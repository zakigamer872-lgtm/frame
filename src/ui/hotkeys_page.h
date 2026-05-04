#pragma once
#include <QWidget>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QPushButton>

class HotkeysPage : public QWidget {
    Q_OBJECT
public:
    explicit HotkeysPage(QWidget* parent = nullptr);

private:
    void setupUI();
    void loadHotkeys();
    void saveHotkeys();

    QKeySequenceEdit* m_startStopEdit;
    QKeySequenceEdit* m_pauseEdit;
    QKeySequenceEdit* m_screenshotEdit;
    QKeySequenceEdit* m_cancelEdit;
    QKeySequenceEdit* m_editorEdit;
};
