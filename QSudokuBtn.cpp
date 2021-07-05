#include "QSudokuBtn.h"

QSudokuBtn::QSudokuBtn(QWidget* parent) : QPushButton(parent) {

}

void QSudokuBtn::set_style(char* p, int num, int x, int y) {
	static char* fixed[] = {"\
						color: black;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						", "\
						color: black;\
						background-color: #eee;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						"
						};
	static char* unfixed[] = {"\
						color: blue;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						","\
						color: blue;\
						background-color: #eee;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						"};
	static char* hint[] = {"\
						color: green;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						","\
						color: green;\
						background-color: #eee;\
						font: bold 18px;\
						min-width: 2em;\
						min-height: 2px;\
						"};
	if (num > 0) {
		setText(QString::number(num));
	}
	if (num == 0) {
		setText(" ");
	}
	int mode = 0;
	if (x == y || x + y == 8) {
		mode = 1;
	}
	if (*p == 'f') {
		this->setStyleSheet(fixed[mode]);
	}
	else if (*p == 'h') {
		this->setStyleSheet(hint[mode]);
	}
	else {
		this->setStyleSheet(unfixed[mode]);
	}
}