#include "tools_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QMessageBox>

ToolsPage::ToolsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ToolsPage::setupUI() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(24, 24, 24, 24);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll);

    auto* content = new QWidget();
    scroll->setWidget(content);
    auto* layout = new QVBoxLayout(content);
    layout->setSpacing(20);

    auto* titleLabel = new QLabel("🔧  Tools");
    QFont tf; tf.setFamily("Segoe UI"); tf.setPointSize(18); tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");
    layout->addWidget(titleLabel);

    auto* subLabel = new QLabel("Powerful editing and processing tools — all in one place.");
    subLabel->setStyleSheet("color: #64748B; font-size: 13px;");
    layout->addWidget(subLabel);

    struct Tool { QString icon, name, description; bool available; };
    QList<Tool> tools = {
        {"✂",  "Video Trimmer",       "Cut and trim recordings with frame-level precision",    true},
        {"T+", "Text Overlay",         "Add animated text, titles, and captions to videos",     true},
        {"🎵", "Music & Audio",        "Add background music and mix audio tracks",             true},
        {"⊛",  "Blur / Redact",       "Blur faces, screens, or sensitive regions",             true},
        {"⇄",  "Format Converter",    "Convert between MP4, MKV, AVI, WebM, and more",        true},
        {"🎙", "Voice to Text (AI)",  "AI-powered speech-to-text transcription",              true},
        {"🎞", "GIF Creator",          "Convert clips to high-quality animated GIFs",           true},
        {"📐", "Crop & Resize",        "Crop to any ratio: 16:9, 9:16, 1:1, 4:3",             true},
        {"🎨", "Color Correction",     "Adjust brightness, contrast, saturation, warmth",       true},
        {"✨", "AI Highlights",        "Auto-detect best moments from long recordings",         false},
        {"🔊", "Audio Mixer",          "Mix system, mic, and music with per-track controls",   true},
        {"📤", "Export & Share",       "Publish directly to YouTube, Twitter, and more",       false},
    };

    auto* grid = new QGridLayout();
    grid->setSpacing(14);

    for (int i = 0; i < tools.size(); ++i) {
        auto& t = tools[i];
        auto* card = new QFrame(content);
        card->setStyleSheet(
            QString("QFrame { background: #0F0F1F; border: 1px solid %1; border-radius: 12px; }"
                    "QFrame:hover { background: #12122A; border-color: #7C3AED; }")
            .arg(t.available ? "#1E1B3A" : "#1A1A2E"));
        card->setCursor(t.available ? Qt::PointingHandCursor : Qt::ArrowCursor);
        card->setFixedHeight(116);

        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 14, 16, 12);
        cl->setSpacing(5);

        auto* topRow = new QHBoxLayout();
        auto* iconLabel = new QLabel(t.icon);
        QFont iFont; iFont.setPointSize(20);
        iconLabel->setFont(iFont);

        if (!t.available) {
            auto* badge = new QLabel("SOON");
            badge->setStyleSheet(
                "background: #1A1A2E; color: #475569; border: 1px solid #2D2B55; "
                "border-radius: 6px; font-size: 9px; padding: 2px 6px; font-weight: bold;");
            topRow->addWidget(iconLabel);
            topRow->addStretch();
            topRow->addWidget(badge);
        } else {
            topRow->addWidget(iconLabel);
            topRow->addStretch();
        }

        auto* nameLabel = new QLabel(t.name);
        QFont nf; nf.setFamily("Segoe UI"); nf.setPointSize(12); nf.setBold(true);
        nameLabel->setFont(nf);
        nameLabel->setStyleSheet(t.available ? "color: #E2E8F0;" : "color: #475569;");

        auto* descLabel = new QLabel(t.description);
        descLabel->setStyleSheet("color: #64748B; font-size: 11px;");
        descLabel->setWordWrap(true);

        cl->addLayout(topRow);
        cl->addWidget(nameLabel);
        cl->addWidget(descLabel);

        if (t.available) {
            const QString name = t.name;
            auto* btn = new QPushButton(card);
            btn->setFixedSize(24, 24);
            btn->setGeometry(card->width() - 34, 8, 24, 24);
            Q_UNUSED(btn)
            card->installEventFilter(this);
            connect(new QObject(card), &QObject::destroyed, [](){});
        }

        grid->addWidget(card, i / 3, i % 3);
    }

    layout->addLayout(grid);
    layout->addStretch();
}
