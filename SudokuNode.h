#pragma once
class SudokuNode    //数独的一个局面
{
public:
    int cost_so_far, cost;
    char num[81];
    bool mark[81][10];
    char num_can_put[81];
    //运算符重载
    friend bool operator<(SudokuNode n1, SudokuNode n2) {
        return n1.cost > n2.cost;
    }
    SudokuNode();
    SudokuNode(char src[][9]);
    //运算函数
    static void mark_cell(int cell, bool* mark, char* num); //标记cell可放数字
    int fill(int cell, int num_fill);    //填数，0代表无解
    int dis_uni();  //填入所有有唯一解的格子
    void cal_cost(int increase);    //计算cost_to_end和cost总
    void get_current_num(char to_num[][9]); //将当前结点信息输出
};
