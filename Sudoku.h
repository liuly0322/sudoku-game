#pragma once

#include <QtWidgets/QMainWindow>
#include <qlineedit.h>
#include <qcombobox.h>
#include <mutex>
#include "QSudokuBtn.h"
#include "SudokuNode.h"

#define MAX_THREAD 32
#define MAX_COUNT 1000000

class Sudoku : public QMainWindow
{
    Q_OBJECT

public:

    //主窗口
    Sudoku(QWidget* parent = Q_NULLPTR);

    //绘图
    void paintEvent(QPaintEvent*);
    QPainter* paint;

    //控件
    QLineEdit* input_num_given; //输入框（输入生成数独时给定数字）
    QComboBox* combox_search;   //搜索模式选择
    QPushButton* btn[11];   //操作按钮
    QSudokuBtn* sudoku_btn[9][9];   //9*9数独按钮
    QSudokuBtn* sudoku_input[10];   //数独输入按钮(0-9，0代表归零)

    //数据
    char sudoku_num[9][9] = { 0 };  //数独当前状态数字
    char sudoku_solu[9][9] = { 0 };    //数独解
    bool fixed[9][9] = { 0 };   //是否被固定（是否是初始状态）
    char hint_info[3];   //hint相关信息，第一位表示之前是否有hint，二，三位置是上一个hint的位置
    char num_to_input = 0;  //缓存待填数字
    char solve_res = 0;  //是否需要再解，避免重复计算，1，2是储存的返回值

    //多线程相关
    int res_num = 0;    //一共有多少个解
    int new_thread = 0; //当前子线程数
    std::thread* next[MAX_THREAD];  //存储子线程
    const int max_thread = MAX_THREAD; //最大线程数
    std::mutex mut_res_num, mut_out;    //互斥锁 确保线程安全
    QTextStream* file_out;   //文件输出流

    //槽函数
    void search_mode_change();  //更改搜索模式
    void regenerate();    //生成数独，不保证有解，并绘制
    void genrt_with_solu(); //生成数独，保证有解，并绘制
    void replay();  //回到初始状态
    void lock();    //固定当前数独
    void solve();   //解数独
    void hint();    //提示
    void if_solu(); //是否有解
    void about();   //关于
    void sudoku_click(int x, int y);    //填入数独
    void save();  //保存当前数独
    void load();  //加载数独
    void all_solu();    //显示所有解答

    //图形化辅助函数
    void initial();    //初始界面创建
    void create_widget(char* title, char* content); //创建错误/提示窗口
    void sudoku_paint();    //数独数字全部更新

    //计算函数
    int search();  //搜索函数，无解时返回0，有解时确保sudoku_solve中是解
    int search_dfs(int cell);  //朴素dfs，无解时返回0
    int search_dfs_ID(SudokuNode node_now, int depth, bool full_search);  //迭代加深dfs
    int* search_dfs_get_tree(SudokuNode node_now, int depth);  //获得树的结构
    void search_dfs_all(SudokuNode node_now, int depth, int recur_depth, bool if_parent);  //dfs输出全部解
    void output_to_file(SudokuNode& node);  //输出到文件
    int search_astar();    //A*搜索算法
    void generate(int size);    //生成数独，不保证有解

    //错误处理
    int if_repeat_error(int i, int j, int x);    //是否有数字重复错误
    void repeat_error(int i, int j);    //i行j列数字重复，报错
};
