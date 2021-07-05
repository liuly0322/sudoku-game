#include "Sudoku.h"
#include <QtWidgets/QApplication>
#include <qtextcodec.h>

int main(int argc, char *argv[])
{
    //防止文件名中文乱码
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    //指定应用程序
    QApplication a(argc, argv);

    //全局字体设置
    QFont font;
    font.setPointSize(14);
    font.setFamily("黑体");
    a.setFont(font);

    //主要窗口对象
    Sudoku w;
    w.show();

    //让应用程序对象进入消息循环（代码阻塞）
    return a.exec();
}
