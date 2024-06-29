#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>
#include <QTimer>

class MarqueeLabel : public QLabel {
    Q_OBJECT

public:
    explicit MarqueeLabel(QWidget *parent = nullptr);

    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateTextPosition();

private:
    QString displayedText;
    int textPosition;
    QTimer *timer;
};

#endif // MARQUEELABEL_H
