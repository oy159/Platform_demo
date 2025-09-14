// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CALLOUT_H
#define CALLOUT_H

#include <QFont>
#include <QGraphicsItem>
#include <QChart>

QT_FORWARD_DECLARE_CLASS(QChart)
QT_FORWARD_DECLARE_CLASS(QGraphicsSceneMouseEvent)

class Callout : public QGraphicsItem
{
public:
    Callout(QChart *parent);
    ~Callout(); // 添加析构函数声明

    void setText(const QString &text);
    void setAnchor(QPointF point);
    void updateGeometry();

    bool isAnchorVisible() const;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setMarkerColor(QColor color);
    QColor getColor() const;
    void setIfDrawColor(bool status);
    bool isIfDrawColor() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
    QChart *m_chart = nullptr;
    QColor m_color = Qt::red;      // 默认marker颜色
    bool IfDrawMarker;       // 是否绘制marker
};

#endif
