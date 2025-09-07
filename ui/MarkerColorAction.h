// MarkerColorAction.h
#pragma once
#include <QWidget>
#include <QWidgetAction>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QColor>

class MarkerColorAction : public QWidgetAction {
    Q_OBJECT
public:
    MarkerColorAction(const QString& labelText, QColor color, QObject* parent = nullptr);

    QColor color() const;
    bool isChecked() const;
    void setChecked(bool checked);

    signals:
        void colorChanged(QColor newColor);
    void checkedChanged(bool checked);

private slots:
    void chooseColor();
    void onCheckedChanged(bool checked);

private:
    QLabel* m_label;
    QCheckBox* m_checkBox;
    QPushButton* m_colorButton;
    QColor m_color;
};