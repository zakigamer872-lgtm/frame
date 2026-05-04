#pragma once
#include <QWidget>

class ToolsPage : public QWidget {
    Q_OBJECT
public:
    explicit ToolsPage(QWidget* parent = nullptr);

private:
    void setupUI();
};
