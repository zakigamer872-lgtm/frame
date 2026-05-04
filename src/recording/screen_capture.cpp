#include "screen_capture.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>

ScreenCapture::ScreenCapture(QObject* parent) : QObject(parent) {
    m_ffmpegProcess = new QProcess(this);
    m_ffmpegProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_ffmpegProcess, &QProcess::errorOccurred,
            [this](QProcess::ProcessError) {
        emit error("FFmpeg: " + m_ffmpegProcess->errorString());
    });
}

ScreenCapture::~ScreenCapture() { stop(); }

void ScreenCapture::start() {
    if (m_running) return;
    m_lastArgs = buildFFmpegArgs();
    startProcess(m_lastArgs);
    m_running = true;
    emit started();
}

void ScreenCapture::stop() {
    if (!m_running) return;
    if (m_ffmpegProcess->state() == QProcess::Running) {
        m_ffmpegProcess->write("q\n");
        if (!m_ffmpegProcess->waitForFinished(5000))
            m_ffmpegProcess->kill();
    }
    m_running = false;
    emit stopped();
}

void ScreenCapture::pause() {
#ifdef Q_OS_WIN
    if (m_ffmpegProcess->state() == QProcess::Running)
        m_ffmpegProcess->write("p");
#endif
}

void ScreenCapture::resume() {
#ifdef Q_OS_WIN
    if (m_ffmpegProcess->state() == QProcess::Running)
        m_ffmpegProcess->write("p");
#endif
}

QStringList ScreenCapture::buildFFmpegArgs() const {
    QStringList args;
    args << "-y" << "-loglevel" << "warning";

    // Resolution
    QString size;
    if      (m_quality == "720p")  size = "1280x720";
    else if (m_quality == "2K")    size = "2560x1440";
    else if (m_quality == "4K")    size = "3840x2160";
    else                           size = "1920x1080";

    // Encoder
    bool isNVENC = m_encoder.contains("NVENC");
    bool isAMD   = m_encoder.contains("AMD");
    bool isIntel = m_encoder.contains("Intel");
    bool isH265  = m_encoder.contains("H.265") || m_encoder.contains("HEVC");
    QString codec;
    if      (isNVENC)  codec = isH265 ? "hevc_nvenc" : "h264_nvenc";
    else if (isAMD)    codec = isH265 ? "hevc_amf"   : "h264_amf";
    else if (isIntel)  codec = isH265 ? "hevc_qsv"   : "h264_qsv";
    else               codec = isH265 ? "libx265"    : "libx264";

#ifdef Q_OS_WIN
    args << "-f" << "gdigrab"
         << "-framerate" << QString::number(m_fps)
         << "-draw_mouse" << "1";
    if (m_captureMode == "window") {
        args << "-i" << ("hwnd=" + QString::number(m_windowId));
    } else if (m_captureMode == "custom" && !m_region.isNull()) {
        args << "-offset_x" << QString::number(m_region.x())
             << "-offset_y" << QString::number(m_region.y())
             << "-video_size" << QString("%1x%2").arg(m_region.width()).arg(m_region.height())
             << "-i" << "desktop";
    } else {
        args << "-i" << "desktop";
    }
#elif defined(Q_OS_LINUX)
    args << "-f" << "x11grab"
         << "-r" << QString::number(m_fps)
         << "-i" << ":0.0";
#endif

    args << "-vcodec" << codec
         << "-s" << size
         << "-r" << QString::number(m_fps);

    if (m_gpuAccel && (isNVENC || isAMD || isIntel)) {
        args << "-preset" << "p4" << "-b:v" << "8M";
    } else {
        args << "-preset" << "ultrafast" << "-crf" << "18";
    }

    if      (m_format == "MKV") args << "-f" << "matroska";
    else if (m_format == "AVI") args << "-f" << "avi";
    else args << "-f" << "mp4" << "-movflags" << "+faststart";

    args << m_outputFile;
    return args;
}

void ScreenCapture::startProcess(const QStringList& args) {
    qDebug() << "[ScreenCapture] ffmpeg" << args.join(" ");
    m_ffmpegProcess->start("ffmpeg", args);
}
