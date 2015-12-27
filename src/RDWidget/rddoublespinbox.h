/*
 * =====================================================================================
 *
 *
 *    Description:
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  HuanJianChao (), hjc0720@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef RDDOUBLESPINBOX_H
#define RDDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QSpinBox>

class RDDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit RDDoubleSpinBox(QWidget *parent = 0);
    void updateValue(double fValue);

signals:
    void valueChanging(double);

protected:
    bool m_bUpdateValue;

};

class RDSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit RDSpinBox(QWidget *parent = 0);
    void updateValue(int fValue);

signals:
    void valueChanging(int);

protected:
    bool m_bUpdateValue;
};

#endif // RDDOUBLESPINBOX_H
