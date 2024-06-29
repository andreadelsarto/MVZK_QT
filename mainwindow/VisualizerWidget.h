#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QAudioDecoder>
#include <QSlider>
#include <fftw3.h>
#include <vector>

class VisualizerWidget : public QWidget {
    Q_OBJECT

public:
    VisualizerWidget(QSlider *slider, QWidget *parent = nullptr);

    void setBaseColor(const QColor &color);
    void start();
    void stop();
    void setAudioSource(const QString &filePath);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();
    void processBuffer();

private:
    QColor baseColor;
    int amplitude;
    int frequency;
    int phase;
    QTimer *timer;
    QAudioDecoder *decoder;
    QSlider *linkedSlider;
    std::vector<double> fftOutput;
    int sampleSize;
    fftw_plan plan;
};

#endif // VISUALIZERWIDGET_H
