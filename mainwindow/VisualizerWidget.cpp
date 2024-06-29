#include "VisualizerWidget.h"
#include <QPainter>
#include <QAudioBuffer>
#include <QUrl>
#include <cmath>

VisualizerWidget::VisualizerWidget(QSlider *slider, QWidget *parent)
: QWidget(parent), linkedSlider(slider), amplitude(20), frequency(2), phase(0), sampleSize(1024) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VisualizerWidget::updateAnimation);

    decoder = new QAudioDecoder(this);
    connect(decoder, &QAudioDecoder::bufferReady, this, &VisualizerWidget::processBuffer);

    fftOutput.resize(sampleSize);
    plan = fftw_plan_r2r_1d(sampleSize, fftOutput.data(), fftOutput.data(), FFTW_R2HC, FFTW_ESTIMATE);
}

void VisualizerWidget::setBaseColor(const QColor &color) {
    baseColor = color;
}

void VisualizerWidget::start() {
    timer->start(30);
}

void VisualizerWidget::stop() {
    timer->stop();
    decoder->stop();
    fftw_destroy_plan(plan);
}

void VisualizerWidget::setAudioSource(const QString &filePath) {
    decoder->setSource(QUrl::fromLocalFile(filePath));
    decoder->start();
}

void VisualizerWidget::processBuffer() {
    QAudioBuffer buffer = decoder->read();
    const qint16 *data = buffer.constData<qint16>();
    for (int i = 0; i < sampleSize; ++i) {
        fftOutput[i] = data[i];
    }

    fftw_execute(plan);
}

void VisualizerWidget::paintEvent(QPaintEvent *event) {
    // Non facciamo nulla qui
}

void VisualizerWidget::updateAnimation() {
    phase += frequency;
    if (phase >= 360) {
        phase -= 360;
    }

    // Aggiorna lo stile del QSlider
    float scale = 1.0 + amplitude * std::abs(fftOutput[1]) / 1000.0;
    QColor newColor = baseColor;
    newColor.setAlphaF(scale);
    QString sliderStyle = QString("QSlider::groove:horizontal { background: %1; height: 8px; }"
    "QSlider::handle:horizontal { background: #aaa; width: 20px; margin: -6px 0; border-radius: 10px; }"
    "QSlider::sub-page:horizontal { background: %2; }")
    .arg(newColor.darker().name(), newColor.name());
    linkedSlider->setStyleSheet(sliderStyle);

    update();
}
