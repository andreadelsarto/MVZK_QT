#include "MarqueeLabel.h"
#include <QPainter>

MarqueeLabel::MarqueeLabel(QWidget *parent) : QLabel(parent), textPosition(0) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MarqueeLabel::updateTextPosition);
    timer->start(30); // Modifica la velocità del testo cambiando l'intervallo del timer
}

void MarqueeLabel::setText(const QString &text) {
    QLabel::setText(text);
    displayedText = text;
    textPosition = width();
}

void MarqueeLabel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int textWidth = fontMetrics().horizontalAdvance(displayedText);
    if (textWidth < width()) {
        // If text is shorter than the label width, just draw it normally
        painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, displayedText);
    } else {
        // Otherwise, draw the scrolling text
        painter.drawText(textPosition, height() / 2 + fontMetrics().ascent() / 2, displayedText);
        painter.drawText(textPosition - textWidth - 10, height() / 2 + fontMetrics().ascent() / 2, displayedText);
    }
}

void MarqueeLabel::updateTextPosition() {
    int textWidth = fontMetrics().horizontalAdvance(displayedText);
    textPosition -= 1; // Modifica la velocità del testo cambiando questo valore
    if (textPosition < -textWidth) {
        textPosition = width();
    }
    update();
}
