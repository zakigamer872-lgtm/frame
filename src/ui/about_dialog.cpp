#include "about_dialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QFrame>
#include <QHBoxLayout>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("About FrameX");
    setFixedSize(420, 480);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* frame = new QFrame(this);
    frame->setStyleSheet(
        "QFrame { background: #0F0F1F; border: 1px solid #2D2B55; border-radius: 18px; }");
    auto* fl = new QVBoxLayout(frame);
    fl->setContentsMargins(28, 28, 28, 28);
    fl->setSpacing(14);
    layout->addWidget(frame);

    // Logo
    auto* logoLabel = new QLabel("🎬  FrameX");
    QFont lf; lf.setFamily("Segoe UI"); lf.setPointSize(26); lf.setBold(true);
    logoLabel->setFont(lf);
    logoLabel->setStyleSheet("color: #C084FC;");
    logoLabel->setAlignment(Qt::AlignCenter);
    fl->addWidget(logoLabel);

    auto* versionLabel = new QLabel("Version 2.0.0");
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet(
        "background: #1E1B3A; color: #7C3AED; border-radius: 10px; "
        "padding: 4px 16px; font-size: 12px; font-weight: bold;");
    fl->addWidget(versionLabel, 0, Qt::AlignCenter);

    auto* tagLabel = new QLabel("Capture everything. Share anything.");
    tagLabel->setAlignment(Qt::AlignCenter);
    tagLabel->setStyleSheet("color: #64748B; font-style: italic;");
    fl->addWidget(tagLabel);

    auto* divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background: #1E1B3A; max-height: 1px; border: none;");
    fl->addWidget(divider);

    struct InfoRow { QString key; QString val; };
    QList<InfoRow> rows = {
        {"Developer",     "ZAKI"},
        {"Instagram",     "@zg22x"},
        {"TikTok",        "@zg22x"},
        {"Framework",     "Qt 6  /  C++17"},
        {"Video Engine",  "FFmpeg"},
        {"Build",         "v2.0.0  —  2024"},
        {"License",       "Free to use"},
    };

    for (auto& r : rows) {
        auto* row = new QHBoxLayout();
        auto* kl = new QLabel(r.key + ":");
        kl->setStyleSheet("color: #475569; font-size: 12px;");
        kl->setFixedWidth(110);
        auto* vl = new QLabel(r.val);
        vl->setStyleSheet("color: #E2E8F0; font-size: 12px; font-weight: bold;");
        row->addWidget(kl);
        row->addWidget(vl);
        row->addStretch();
        fl->addLayout(row);
    }

    fl->addStretch();

    auto* div2 = new QFrame();
    div2->setFrameShape(QFrame::HLine);
    div2->setStyleSheet("background: #1E1B3A; max-height: 1px; border: none;");
    fl->addWidget(div2);

    auto* socialRow = new QHBoxLayout();
    const QString btnStyle =
        "QPushButton { background: #12122A; border: 1px solid #2D2B55; border-radius: 10px; "
        "color: #CBD5E1; padding: 9px 18px; font-size: 13px; }"
        "QPushButton:hover { background: #7C3AED; border-color: #9333EA; color: white; }";

    auto* igBtn = new QPushButton("📸  Instagram");
    igBtn->setStyleSheet(btnStyle);
    igBtn->setCursor(Qt::PointingHandCursor);
    connect(igBtn, &QPushButton::clicked, [](){
        QDesktopServices::openUrl(QUrl("https://instagram.com/zg22x"));
    });

    auto* tkBtn = new QPushButton("🎵  TikTok");
    tkBtn->setStyleSheet(btnStyle);
    tkBtn->setCursor(Qt::PointingHandCursor);
    connect(tkBtn, &QPushButton::clicked, [](){
        QDesktopServices::openUrl(QUrl("https://tiktok.com/@zg22x"));
    });

    socialRow->addWidget(igBtn);
    socialRow->addWidget(tkBtn);
    fl->addLayout(socialRow);

    auto* closeBtn = new QPushButton("Close");
    closeBtn->setObjectName("primaryBtn");
    closeBtn->setFixedHeight(40);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    fl->addWidget(closeBtn);
}
