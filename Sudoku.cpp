#include "Sudoku.h"
#include "QSudokuBtn.h"
#include "SudokuNode.h"
#include <qdialog.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <queue>

//创建弹出窗口
void Sudoku::create_widget(char* title, char* content) {
    QDialog* no_solution = new QDialog;
    no_solution->setFixedSize(300, 100);
    no_solution->setWindowTitle(title);
    QLabel* no_solution_lable = new QLabel(no_solution);
    no_solution_lable->setText(content);
    no_solution_lable->setFixedSize(300, 100);
    no_solution_lable->setAlignment(Qt::AlignCenter);
    no_solution_lable->setStyleSheet("background-color: #fff");
    no_solution->show();
}

//错误处理
int Sudoku::if_repeat_error(int i, int j, int x) {
    //是否有冲突
    for (int k = 0; k < 9; k++) {
        if (x == sudoku_num[i][k]) {
            repeat_error(i, k);
            return 1;
        }
    }
    for (int k = 0; k < 9; k++) {
        if (x == sudoku_num[k][j]) {
            repeat_error(k, j);
            return 1;
        }
    }
    if (i == j) {
        for (int k = 0; k < 9; k++) {
            if (x == sudoku_num[k][k]) {
                repeat_error(k, k);
                return 1;
            }
        }
    }
    if (i + j == 8) {
        for (int k = 0; k < 9; k++) {
            if (x == sudoku_num[k][8 - k]) {
                repeat_error(k, 8 - k);
                return 1;
            }
        }
    }
    int i_start = i / 3 * 3, j_start = j / 3 * 3;
    for (int di = 0; di < 3; di++) {
        for (int dj = 0; dj < 3; dj++) {
            if (x == sudoku_num[i_start + di][j_start + dj]) {
                repeat_error(i_start + di, j_start + dj);
                return 1;
            }
        }
    }
    return 0;
}

void Sudoku::repeat_error(int i, int j) {
    if (i < 0 || i > 8 || j < 0 || j > 8) {
        return;
    }
    char dest[100] = { 0 };
    snprintf(dest, sizeof(dest), "第%d行%d列重复", i + 1, j + 1);
    create_widget("错误", dest);
}

//绘制各块边界线
void Sudoku::paintEvent(QPaintEvent*) {
    paint = new QPainter;
    paint->begin(this);
    paint->setPen(QPen(Qt::black, 2));
    for (int i = 0; i <= 3; i++) {
        paint->drawLine(150 * i, 0, 150 * i, 450);
        paint->drawLine(0, 150 * i, 450, 150 * i);
    }
    paint->end();
}

//创建主界面
void Sudoku::initial() {
    //输入数字
    QLabel* lable_initial_num = new QLabel(this);
    lable_initial_num->setText("给定数目:");
    lable_initial_num->move(500, 0);
    lable_initial_num->setStyleSheet("font: 14px");
    input_num_given = new QLineEdit(this);
    input_num_given->setText("17"); //默认给定数目
    input_num_given->move(570, 0);
    input_num_given->resize(80, 30);
    input_num_given->setAlignment(Qt::AlignRight);

    //搜索模式选择
    QLabel* lable_search = new QLabel(this);
    lable_search->setText("搜索模式:");
    lable_search->move(500, 30);
    lable_search->setStyleSheet("font: 14px");
    combox_search = new QComboBox(this);
    QStringList search_mode_str;
    search_mode_str << "DFS" << "IDDFS" << "A*算法";
    combox_search->addItems(search_mode_str);
    combox_search->setStyleSheet("font: 14px");
    combox_search->setCurrentIndex(2);
    combox_search->move(570, 30);
    combox_search->resize(80, 30);
    connect(combox_search, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &Sudoku::search_mode_change);

    //分别批量创建功能按钮，数独数字按钮，填入数字按钮
    //功能按钮
    char* btn_str[] = { "重新生成","生成有解数独","提示","解数独","当前是否有解","重新开始","保存","加载","锁定","输出全部解","关于" };
    void (Sudoku:: * f[])() = { &Sudoku::regenerate,&Sudoku::genrt_with_solu,&Sudoku::hint,&Sudoku::solve,
        &Sudoku::if_solu,&Sudoku::replay,&Sudoku::save,&Sudoku::load,&Sudoku::lock,&Sudoku::all_solu,&Sudoku::about };
    for (int i = 0; i < 11; i++) {
        btn[i] = new QPushButton(btn_str[i], this);
        btn[i]->move(500, i * 30 + 60);
        btn[i]->resize(150, 30);
        btn[i]->setStyleSheet("font: 14px");
        connect(btn[i], &QPushButton::clicked, this, f[i]);
    }

    //数独数字按钮
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudoku_btn[i][j] = new QSudokuBtn(this);
            sudoku_btn[i][j]->resize(46, 46);
            sudoku_btn[i][j]->move(50 * j + 2, 50 * i + 2);
            connect(sudoku_btn[i][j], &QPushButton::clicked, this, [=]() {sudoku_click(i, j); });
        }
    }

    //填入数字按钮
    for (int i = 0; i <= 9; i++) {
        sudoku_input[i] = new QSudokuBtn(this);
        sudoku_input[i]->set_style("fixed", i, 0, 1);
        sudoku_input[i]->resize(45, 45);
        sudoku_input[i]->move(i * 45, 500);
        if (i == 0) {
            sudoku_input[0]->setText("X");
            connect(sudoku_input[0], &QPushButton::clicked, this, [=]() {num_to_input = 127; });
        }
        else {
            connect(sudoku_input[i], &QPushButton::clicked, this, [=]() {num_to_input = i; });
        }
    }
    setStyleSheet("background-color: #fff");
    setFixedSize(650, 550);
    setWindowTitle("数独");
}

//关于界面的绘制
void Sudoku::about() {
    char about_content[] = "\
关于：本程序可用于数独的求解，游玩\n\n\
功能说明：\n\
1.重新生成：生成数独，给定数字数取输入框\n\
2.生成有解数独：同上，但保证数独有解\n\
3.锁定：当前所有数字均被锁定\n\
4.提示：随机给出一个未填的位置的数字\n\
5.解数独：全部求解\n\
6.当前是否有解：判断当前局面是否可解\n\
7.重新开始：回到初始状态\n\
8.输出全部解：到某个指定文件夹中\n\n\
操作说明：\n\
1.点选下方数字，再点击要填入的格子即可\n\
X起到删除的作用\n\
2.通过生成初始0数字的数独，配合锁定，\n\
可以输入自制数独\n\
3.也可以通过存档读档的方式导入自制数独\n\n\
存档说明：\n\
包含两个9*9的方阵，第一个记录数独每一个数字，\n\
第二个记录该位置是不是初始给定的数字。\n\
注：初始给定的数字，对应为非0（存档填入1）。\n\
可以通过重复两次相同数独达到初始化的目的。";
    QDialog* dialog = new QDialog;
    dialog->setFixedSize(500, 400);
    dialog->setWindowTitle("关于");
    QTextBrowser* about_text = new QTextBrowser(dialog);
    about_text->setText(about_content);
    about_text->setFixedSize(500, 400);
    about_text->setStyleSheet("background: #fff");
    dialog->show();
}

void Sudoku::search_mode_change() {
    solve_res = 0;
    if (combox_search->currentIndex() == 0) {
        create_widget("提示", "dfs搜索可能较慢");
    }
}

//点击数独数字
void Sudoku::sudoku_click(int x, int y) {
    if (num_to_input == 0) {    //意味着是否需要更改
        return;
    }
    //先判断这一输入是否合法
    if (fixed[x][y]) {  //是否被固定
        num_to_input = 0;
        create_widget("错误", "不能修改固定的数字");
        return;
    }
    if (num_to_input != 127 && if_repeat_error(x, y, num_to_input)) {
        //是否矛盾，如果是127（删除）就不需要判断
        return;
    }
    //现在可以输入，如果有hint先清除
    if (hint_info[0]) {
        sudoku_btn[hint_info[1]][hint_info[2]]->set_style("unfixed", -1, hint_info[1], hint_info[2]);
        hint_info[0] = 0;
    }
    if (num_to_input != 127) {
        sudoku_num[x][y] = num_to_input;
    } else {
        sudoku_num[x][y] = 0;
    }
    solve_res = 0; num_to_input = 0;   //释放输入缓存
    sudoku_btn[x][y]->set_style("unfixed", sudoku_num[x][y], x, y);
}

//更新数独数字（全部）
void Sudoku::sudoku_paint() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (fixed[i][j]) {
                sudoku_btn[i][j]->set_style("fixed", sudoku_num[i][j], i, j);
            } else {
                sudoku_btn[i][j]->set_style("unfixed", sudoku_num[i][j], i, j);
            }
        }
    }
}

//生成初始具有size个数字的数独（不保证有解）
void Sudoku::generate(int size) {
    solve_res = 0;
    if (size < 0 || size > 40) {
        create_widget("错误", "请输入0-40之间的数字");
        return;
    }
    //先重置fixed和num
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            fixed[i][j] = false;
            sudoku_num[i][j] = 0;
        }
    }
    bool mark[10];
    for (int i = 0; i < size; i++) {
        int cell_now = rand() % 81;
        if (fixed[cell_now / 9][cell_now % 9] == true) {
            i--;
            continue;
        }
        for (int j = 1; j <= 9; j++) {
            mark[j] = false;
        }
        SudokuNode::mark_cell(cell_now, mark, sudoku_num[0]);
        char queue[9];
        int num_can_place = 0;  //queue存储所有可以放的数字
        for (int j = 1; j <= 9; j++) {
            if (mark[j]) {
                continue;
            }
            queue[num_can_place] = j;
            num_can_place++;
        }
        if (num_can_place) {
            sudoku_num[cell_now / 9][cell_now % 9] = queue[rand() % num_can_place];
            fixed[cell_now / 9][cell_now % 9] = true;   //标记
            continue;
        }
        i--;
    }
    hint_info[0] = 0;
}

//重新生成：生成并绘图
void Sudoku::regenerate() {
    generate(input_num_given->text().toInt());
    sudoku_paint();
}

//生成有解数独
void Sudoku::genrt_with_solu() {
    do {
        generate(17);
    } while (!search());
    int size = input_num_given->text().toInt();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudoku_num[i][j] = sudoku_solu[i][j];
        }
    }
    for (int i = 0; i < 81 - size; i++) {   //挖洞
        int cell = rand() % 81;
        if (!sudoku_num[cell / 9][cell % 9]) {
            i--; continue;
        }
        sudoku_num[cell / 9][cell % 9] = 0;
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            fixed[i][j] = sudoku_num[i][j] ? true : false;
        }
    }
    sudoku_paint();
}

//重新从初始状态开始游戏
void Sudoku::replay() {
    solve_res = 0;  //避免之前无解
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!fixed[i][j]) {
                sudoku_num[i][j] = 0;
            }
        }
    }
    sudoku_paint();
}

//当前数字全部固定
void Sudoku::lock() {
    solve_res = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sudoku_num[i][j]) {
                fixed[i][j] = true;
            }
        }
    }
    sudoku_paint();
}

//搜索函数，返回值1代表有解，0代表无解
int Sudoku::search() {
    if (solve_res) {
        return solve_res - 1;
    }
    //搜索模式：0代表dfs，1代表迭代加深dfs，2代表A*算法
    int mode = combox_search->currentIndex();
    if (mode == 0) {
        solve_res = search_dfs(0) + 1;
    } else if (mode == 1) {
        SudokuNode start_node(sudoku_num);
        if (search_dfs_ID(start_node, 0, false) || search_dfs_ID(start_node, 0, true)) {
            solve_res = 2;
        } else {
            solve_res = 1;
        }
    } else {
        solve_res = search_astar() + 1;
    }
    return solve_res - 1;
}

//dfs搜索
int Sudoku::search_dfs(int cell) {
    //x代表格子，递归出口x=81(0-80格子均被填完）
    if (cell == 0) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                sudoku_solu[i][j] = sudoku_num[i][j];
            }
        }
    }
    if (cell == 81) {
        return 1;
    }
    int x = cell / 9, y = cell % 9; //转换出对应x,y(坐标)
    bool mark[10] = { 0 };  //记录1-9是否合法
    if (sudoku_solu[x][y]) {
        return search_dfs(cell + 1);
    }
    SudokuNode::mark_cell(cell, mark, sudoku_solu[0]);    //标记重复
    for (int i = 1; i <= 9; i++) {
        if (!mark[i]) {
            sudoku_solu[x][y] = i;
            if (search_dfs(cell + 1) == 1) {
                return 1;
            }
            sudoku_solu[x][y] = 0;
        }
    }
    return 0;
}

//迭代加深DFS
int Sudoku::search_dfs_ID(SudokuNode node_now, int depth, bool full_search) {
    if (depth > 24 && !full_search) {
        return 0;	//迭代加深，退出递归。
    }
    SudokuNode node_new;
    int dis_uni = node_now.dis_uni();
    if (dis_uni < 0)return 0;
    if (dis_uni == 0) {
        node_now.get_current_num(sudoku_solu);     //提取当前信息
        return 1;
    }
    dis_uni--;
    for (int j = 1; j <= 9; j++) {  //填入可能最少的格子进入下一层
        if (!node_now.mark[dis_uni][j]) {
            node_new = node_now;
            if (!node_new.fill(dis_uni, j)) {
                continue;
            }
            if (search_dfs_ID(node_new, depth + 1, full_search)) {	//填入
                return 1;
            }
        }
    }
    return 0;
}

int* Sudoku::search_dfs_get_tree(SudokuNode node_now, int depth) {
    static int depth_num[4];    //0,1,2,3层分别有多少个子节点
    if (depth == 0) {
        for (int i = 0; i < 4; i++) {
            depth_num[i] = 0;
        }
    }
    if (depth >= 4) {
        return 0;
    }
    SudokuNode node_new;
    int dis_uni = node_now.dis_uni();
    if (dis_uni <= 0)return 0;
    dis_uni--;
    for (int j = 1; j <= 9; j++) {  //填入可能最少的格子进入下一层
        if (!node_now.mark[dis_uni][j]) {
            node_new = node_now;
            if (!node_new.fill(dis_uni, j)) {
                continue;
            }
            search_dfs_get_tree(node_new, depth + 1);
            depth_num[depth]++;
        }
    }
    return depth_num;
}

//输出全部解
void Sudoku::search_dfs_all(SudokuNode node_now, int depth, int recur_depth, bool if_parent) {
    SudokuNode node_new;
    int dis_uni = node_now.dis_uni();
    if (dis_uni < 0)return;
    if (dis_uni == 0) {
        output_to_file(node_now);
        return;
    }
    dis_uni--;
    for (int j = 1; j <= 9; j++) {  //填入可能最少的格子进入下一层
        if (!node_now.mark[dis_uni][j]) {
            node_new = node_now;
            if (!node_new.fill(dis_uni, j)) {
                continue;
            }
            if (if_parent && depth == recur_depth) {
                next[new_thread++] = new std::thread(&Sudoku::search_dfs_all, this, node_new, depth + 1, 0, false);
            }
            else {
                search_dfs_all(node_new, depth + 1, recur_depth, if_parent);
            }
        }
    }
    return;
}

void Sudoku::output_to_file(SudokuNode& node) {
    int n;
    mut_res_num.lock();
    n = ++res_num;
    mut_res_num.unlock();
    if (n > MAX_COUNT) {
        return;
    }
    char out[164] = { 0 };
    for (int i = 0; i < 81; i++) {
        out[i * 2] = node.num[i] + '0';
    }
    for (int i = 17; i < 162; i += 18) {
        out[i] = '\n';
    }
    for (int i = 0; i < 162; i++) {
        if (!out[i]) {
            out[i] = ' ';
        }
    }
    out[162] = '\n';
    mut_out.lock();
    file_out->operator<<(out);
    mut_out.unlock();
}

//A*搜索
int Sudoku::search_astar() {
    SudokuNode current(sudoku_num), next;
    std::priority_queue<SudokuNode> frontier;
    frontier.push(current);
    while (!frontier.empty()) {
        current = frontier.top();
        frontier.pop();
        //接下来需要遍历所有可能节点。由于相互的连通性，只要从最少可能的找即可
        int min_index = current.dis_uni();
        if (min_index < 0)continue;
        if (min_index == 0) {
            current.get_current_num(sudoku_solu);
            return 1;
        }
        min_index--;
        //对这个格子生成所有可能的新节点，并加入优先队列
        int cost_now = 0;
        for (int j = 1; j <= 9; j++) {
            if (!current.mark[min_index][j]) {
                cost_now++;
            }
        }
        for (int j = 1; j <= 9; j++) {
            if (!current.mark[min_index][j]) {
                next = current;
                if (next.fill(min_index, j)) {
                    next.cal_cost(cost_now);
                    frontier.push(next);
                }
            }
        }
    }
    return 0;
}

//判断当前局面是否有解
void Sudoku::if_solu() {
    if (search()) {
        create_widget("结果", "当前有解");
    } else {
        create_widget("结果", "当前无解");
    }
}

//解当前局面
void Sudoku::solve() {
    if (search()) {   //找到解，输出
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                sudoku_num[i][j] = sudoku_solu[i][j];
            }
        }
        sudoku_paint();
    } else {
        create_widget("错误", "该数独无解");    //无解提示
    }
}

void Sudoku::hint() {
    if (!search()) {  //无解提示
        create_widget("错误", "该数独无解");
        return;
    }
    SudokuNode node_now(sudoku_num);
    int min = 127, min_idx;
    for (int i = 0; i < 81; i++) {
        if (node_now.num_can_put[i] < min) {
            min = node_now.num_can_put[i];
            min_idx = i;
        }
    }
    if (min == 127) return;
	const int condition[29][9] = { {0,10,20,30,40,50,60,70,80}, {8,16,24,32,40,48,56,64,72},
		{0,1,2,9,10,11,18,19,20},{3,4,5,12,13,14,21,22,23},{6,7,8,15,16,17,24,25,26},
		{27,28,29,36,37,38,45,46,47},{30,31,32,39,40,41,48,49,50},{33,34,35,42,43,44,51,52,53},
		{54,55,56,63,64,65,72,73,74},{57,58,59,66,67,68,75,76,77},{60,61,62,69,70,71,78,79,80},
		{0,1,2,3,4,5,6,7,8},{9,10,11,12,13,14,15,16,17},{18,19,20,21,22,23,24,25,26},
		{27,28,29,30,31,32,33,34,35},{36,37,38,39,40,41,42,43,44},{45,46,47,48,49,50,51,52,53},
		{54,55,56,57,58,59,60,61,62},{63,64,65,66,67,68,69,70,71},{72,73,74,75,76,77,78,79,80},
		{0,9,18,27,36,45,54,63,72},{1,10,19,28,37,46,55,64,73},{2,11,20,29,38,47,56,65,74},
		{3,12,21,30,39,48,57,66,75},{4,13,22,31,40,49,58,67,76},{5,14,23,32,41,50,59,68,77},
		{6,15,24,33,42,51,60,69,78},{7,16,25,34,43,52,61,70,79},{8,17,26,35,44,53,62,71,80} };
    for (int i = 0; i < 29; i++) {
        for (int num = 1; num <= 9; num++) {
            int count = 0, id = 0;
            for (int j = 0; j < 9; j++) {
                if (!node_now.num[condition[i][j]] && !node_now.mark[condition[i][j]][num]) {
                    count++; id = condition[i][j];
                }
            }
            if (count == 1) {
                min_idx = id;
                num = 10; i = 30;   //打破循环
            }
        }
    }
    int x = min_idx / 9, y = min_idx % 9;
    //绘制：要考虑之前有没有hint
    if (hint_info[0]) {
        sudoku_btn[hint_info[1]][hint_info[2]]->set_style("unfixed", -1, hint_info[1], hint_info[2]);
    }
    hint_info[0] = 1, hint_info[1] = x, hint_info[2] = y;
    sudoku_num[x][y] = sudoku_solu[x][y];
    sudoku_btn[x][y]->set_style("hint", sudoku_num[x][y], x, y);
}

void Sudoku::save() {
    QString curPath = QDir::currentPath();
    QString file_path = QFileDialog::getSaveFileName(this, "保存存档", curPath, "存档文件(*.txt)");
    QFile File(file_path);
    if (!File.open(QIODevice::WriteOnly | QIODevice::Text)) {   //异常
        File.close();
        return;
    }
    QTextStream out(&File);
    int tmp_num;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            tmp_num = (int)sudoku_num[i][j];
            out << tmp_num << ' ';
        }
        out << endl;
    }
    out << endl;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            tmp_num = (int)fixed[i][j];
            out << tmp_num << ' ';
        }
        out << endl;
    }
    File.close();
}

void Sudoku::load() {
    QString curPath = QDir::currentPath();
    QString file_path = QFileDialog::getOpenFileName(this, "选择一个文件", curPath, "存档文件(*.txt)");
    QFile File(file_path);
    if (!File.open(QIODevice::ReadOnly | QIODevice::Text)) {   //异常
        File.close();
        return;
    }
    QTextStream in(&File);
    int tmp_num;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> tmp_num;
            sudoku_num[i][j] = (char)tmp_num;
        }
    }
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			in >> tmp_num;
			fixed[i][j] = tmp_num ? true : false;
		}
	}
    num_to_input = 0; solve_res = 0; hint_info[0] = 0;
    sudoku_paint();
    File.close();
}

void Sudoku::all_solu() {
    QString curPath = QDir::currentPath();
    QString file_path = QFileDialog::getSaveFileName(this, "保存存档", curPath, "存档文件(*.txt)");
    QFile File(file_path);
    if (!File.open(QIODevice::WriteOnly | QIODevice::Text)) {   //异常
        File.close();
        return;
    }
    file_out = new QTextStream(&File);
    res_num = 0; new_thread = 0;
    SudokuNode start_node(sudoku_num);
    int* p = search_dfs_get_tree(start_node, 0);
    if (!p) {
        search_dfs_all(start_node, 0, 0, false);
    }
    else {
        for (int i = 3; i >= 0; i--) {
            if (p[i] < MAX_THREAD) {
                int tmp = p[i];
                search_dfs_all(start_node, 0, i, true);
                break;
            }
        }
    }
    char output[100];
    for (int i = 0; i < new_thread; i++) {  //阻塞直到每个线程都算完
        next[i]->join();
    }
    for (int i = 0; i < new_thread; i++) {  //回收new出的内存
        delete next[i];
    }
    snprintf(output, 100, "%s%d%s", "共有", res_num, "个解");
    create_widget("提示", output);
    File.close();
}

Sudoku::Sudoku(QWidget *parent)
    : QMainWindow(parent)
{

    //前期处理
    srand((unsigned)time(0));

    //图形化界面创建
    initial();
    //connect之类

    //生成数独，默认大小17，由输入框数字指定
    regenerate();
}
