#include "SudokuNode.h"

void SudokuNode::mark_cell(int cell, bool* mark, char* num) {
    int x = cell / 9, y = cell % 9;
    for (int i = 0; i < 9; i++) {
        mark[num[9 * i + y]] = true;
    }
    for (int i = 0; i < 9; i++) {
        mark[num[9 * x + i]] = true;
    }
    if (x == y) {
        for (int i = 0; i < 9; i++) {
            mark[num[i * 10]] = true;
        }
    }
    if (x + y == 8) {
        for (int i = 0; i < 9; i++) {
            mark[num[8 * i + 8]] = true;
        }
    }
    int i_start = x / 3 * 3, j_start = y / 3 * 3;
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            mark[num[9 * (i_start + dx) + (j_start + dy)]] = true;
        }
    }
}

SudokuNode::SudokuNode() {

}

SudokuNode::SudokuNode(char src[][9]) {
    cost_so_far = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            num[9 * i + j] = src[i][j];
        }
    }
    for (int i = 0; i < 81; i++) {
        for (int j = 0; j < 10; j++) {
            mark[i][j] = false;
        }
    }
    for (int i = 0; i < 81; i++) {
        num_can_put[i] = 0;
    }
    for (int i = 0; i < 81; i++) {
        if (num[i]) {
            num_can_put[i] = 127;	//没有需要安放的了，此时设一个最大值
        }
        else {
            mark_cell(i, mark[i], num);
            for (int j = 1; j <= 9; j++) {
                if (!mark[i][j]) {	//第i个格子可以放数字j
                    num_can_put[i]++;
                }
            }
        }
    }
    cal_cost(0);
}

int SudokuNode::fill(int cell, int num_fill) {
    int x = cell / 9, y = cell % 9;
    num[cell] = num_fill;
    num_can_put[cell] = 127;
    for (int i = 0; i < 9; i++) {
        if (!num[9 * x + i] && !mark[9 * x + i][num_fill]) {
            mark[9 * x + i][num_fill] = true;
            if (--num_can_put[9 * x + i]) {
                continue;
            }
            return 0;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (!num[9 * i + y] && !mark[9 * i + y][num_fill]) {
            mark[9 * i + y][num_fill] = true;
            if (--num_can_put[9 * i + y]) {
                continue;
            }
            return 0;
        }
    }
    if (x == y) {
        for (int i = 0; i < 9; i++) {
            if (!num[10 * i] && !mark[10 * i][num_fill]) {
                mark[10 * i][num_fill] = true;
                if (--num_can_put[10 * i]) {
                    continue;
                }
                return 0;
            }
        }
    }
    if (x + y == 8) {
        for (int i = 0; i < 9; i++) {
            if (!num[8 * i + 8] && !mark[8 * i + 8][num_fill]) {
                mark[8 * i + 8][num_fill] = true;
                if (--num_can_put[8 * i + 8]) {
                    continue;
                }
                return 0;
            }
        }
    }
    int i_start = x / 3 * 3, j_start = y / 3 * 3;
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            if (!num[9 * (i_start + dx) + (j_start + dy)] &&
                !mark[(i_start + dx) * 9 + (j_start + dy)][num_fill]) {
                mark[(i_start + dx) * 9 + (j_start + dy)][num_fill] = true;
                if (--num_can_put[(i_start + dx) * 9 + (j_start + dy)]) {
                    continue;
                }
                return 0;
            }
        }
    }
    return 1;
}

int SudokuNode::dis_uni() {
    for (int i = 0; i < 81; i++) {
        if (num_can_put[i] == 0) {	//无解
            return -1;
        }
    }
    int min, min_index, flag;
    do {
        min = 127, flag = 0;
        for (int i = 0; i < 81; i++) {
            if (num_can_put[i] == 1) {	//唯一解，填入
                flag = 1; cost_so_far++;
                for (int j = 1; j <= 9; j++) {
                    if (!mark[i][j]) {
                        if (!fill(i, j)) {
                            return -1;
                        }
                        break;
                    }
                }
                continue;	//填入唯一解后，再次进入循环
            }
            else if (num_can_put[i] < min) {	//记录最小值
                min = num_can_put[i];
                min_index = i;
            }
        }
    } while (flag == 1);	//填入了唯一解，需要再搜
    if (min == 127)return 0;
    return min_index + 1;    //状态压缩
}

void SudokuNode::cal_cost(int increase) {
    cost_so_far += increase;
    cost = 0;
    for (int i = 0; i < 81; i++) {
        if (num_can_put[i] != 127) {
            cost += num_can_put[i];
        }
    }
    cost += cost_so_far * 4;   //玄学常数
}

void SudokuNode::get_current_num(char to_num[][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            to_num[i][j] = num[9 * i + j];
        }
    }
}