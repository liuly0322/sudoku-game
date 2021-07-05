#pragma once
#include <qpushbutton.h>

class QSudokuBtn :public QPushButton
{
    Q_OBJECT
public:
    explicit QSudokuBtn(QWidget* parent = 0);
    void set_style(char* p, int num, int x, int y);    //设置按钮风格
};