#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>

class ScreenshotsPage : public QWidget {
    Q_OBJECT
public:
    explicit ScreenshotsPage(QWidget* parent = nullptr);
    void loadScreenshots();

private:
    void setupUI();

    QGridLayout* m_gridLayout  = nullptr;
    QWidget*     m_gridWidget  = nullptr;
    QLabel*      m_emptyLabel  = nullptr;
    QLabel*      m_countLabel  = nullptr;
};
