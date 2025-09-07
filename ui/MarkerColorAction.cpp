// MarkerColorAction.cpp
#include "MarkerColorAction.h"
#include <QHBoxLayout>
#include <QColorDialog>

MarkerColorAction::MarkerColorAction(const QString& labelText, QColor color, QObject* parent)
    : QWidgetAction(parent), m_color(color)
{
    QWidget* widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    m_label = new QLabel(labelText, widget);
    m_checkBox = new QCheckBox(widget);
    m_colorButton = new QPushButton(widget);
    m_colorButton->setFixedSize(24, 24);
    m_colorButton->setStyleSheet(QString("background-color: %1; border:1px solid gray;").arg(color.name()));

    connect(m_colorButton, &QPushButton::clicked, this, &MarkerColorAction::chooseColor);
    connect(m_checkBox, &QCheckBox::toggled, this, &MarkerColorAction::onCheckedChanged);

    layout->addWidget(m_checkBox);
    layout->addWidget(m_label);
    layout->addWidget(m_colorButton);

    widget->setLayout(layout);
    setDefaultWidget(widget);
}

QColor MarkerColorAction::color() const {
    return m_color;
}

bool MarkerColorAction::isChecked() const {
    return m_checkBox->isChecked();
}

void MarkerColorAction::setChecked(bool checked) {
    m_checkBox->setChecked(checked);
}

void MarkerColorAction::chooseColor() {
    QColor newColor = QColorDialog::getColor(m_color, nullptr, "选择标记颜色");
    if (newColor.isValid()) {
        m_color = newColor;
        m_colorButton->setStyleSheet(QString("background-color: %1; border:1px solid gray;").arg(m_color.name()));
        emit colorChanged(m_color);
    }
}

void MarkerColorAction::onCheckedChanged(bool checked) {
    emit checkedChanged(checked);
}