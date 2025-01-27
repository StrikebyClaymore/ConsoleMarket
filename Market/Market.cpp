// Market.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <stdlib.h>

using namespace std;

enum COLORS {
	Black = 0,
	Grey = FOREGROUND_INTENSITY,
	LightGrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Blue = FOREGROUND_BLUE,
	Green = FOREGROUND_GREEN,
	Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
	Red = FOREGROUND_RED,
	Purple = FOREGROUND_RED | FOREGROUND_BLUE,
	LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	LightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
	LightPurple = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Orange = FOREGROUND_RED | FOREGROUND_GREEN,
	Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
};

vector<int> idxs = { };

const short cols = 60;
const short rows = 22;
const short char_size = 8;

char map[rows][cols] = {};
vector<COLORS> colors_map[rows][cols] = {};

COORD cursor_position;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO SCRN_INFO;
CONSOLE_CURSOR_INFO structCursorInfo;
HWND window_header = GetConsoleWindow();

int GenId() {
	if ((int)size(idxs) == 0) {
		idxs.push_back(0);
		return 0;
	}
	int id = idxs[(int)size(idxs) - 1] + 1;
	idxs.push_back(id);
	return idxs[(int)size(idxs)-1];
}
void RemoveId(int n) {
	idxs.erase(idxs.begin() + n, idxs.begin() + n + 1);
}

void ChangeCursorPosition(short x, short y) {
	cursor_position.X = x; cursor_position.Y = y;
	SetConsoleCursorPosition(hConsole, cursor_position);
}

void SetWriteColor(WORD color)
{
	SetConsoleTextAttribute(hConsole, color);
}

int GetRandomNumber(int min, int max) {
	if (max < min) {
		return -1;
	}
	return min + rand() % (max - min + 1);
}

class Objects {
public:
	int idx;
	int zIndex;
	COORD position;
	POINT body_size;	
	COLORS color;
};

vector<Objects> objects = { };

class Market : public Objects {
public:

	char body[7][9] = { {'#','#','#','#', '#','#','#','#','#'},
						{'[','M','A','R', 'K','E','T','!',']'},
						{'#','X','X','#', '#','#','X','X','#'},
						{'#','X','X','#', ' ','#','X','X','#'},
						{'#','X','X','#', '#','#','X','X','#'},
						{'#','X','X','X', 'X','X','X','X','#'},
						{'#','#','#','#', '#','#','#','#','#'} };

	Market(short x, short y) {
		position.X = x; position.Y = y;
		zIndex = position.Y;
		idx = GenId();
		color = LightGrey;
		body_size = { (int)size(body[0]), (int)size(body) };
		objects.push_back(*this);
	}
};

bool CanMove(COORD pos, short dir, short speed) {
	switch (dir) {
		case 0: {
			if (pos.Y > 0) { return true; }
			else { return false; }
			break;
		}
		case 1: {
			if (pos.X < cols - 3) { return true; }
			else { return false; }
			break;
		}
		case 2: {
			if (pos.Y < rows - 3) { return true; }
			else { return false; }
			break;
		}
		case 3: {
			if (pos.X > 0) { return true; }
			else { return false; }
			break;
		}
	}
	return false;
}

class Human : public Objects {
public:
	bool life = false;
	int stepTime = 100;
	short dir = 1;
	short speed = 1;
	Objects self;
	char body[3][3] = { {' ','O',' '},
						{'/','|',(char)92},
						{'/',' ',(char)92} };

	char memory[3][3] = { };
	COLORS color_memory = Grey;
	//vector <COLORS> colors_memory[3][3] = { };
	vector<Objects> collider = {};

	Human(short x, short y, short _dir) {
		zIndex = position.Y;
		idx = GenId();
		color = Green;
		body_size = { (int)size(body[0]), (int)size(body) };
		position.X = x; position.Y = y;
		dir = _dir;
		SaveMemory();
		life = true;
		objects.push_back(*this);
		self = *this;
		AI();
	}

	void AI() {
		while (life) {
			Sleep(stepTime);
			MoveAndCollide(dir);
			self = *this;
			Colliding();
		}
	}

	void Colliding() {
		for (int i = 0; i < (int)size(objects); i++) {
			//cout << to_string((int)size(objects));
			if (objects[i].idx == self.idx) { continue; } //objects[i] == self 
			if (Collide(objects[i])) {
				collider.push_back(objects[i]);
				//cout << self.position.X;
				//this->life = false;
			}
		}
	}

	bool Collide(Objects o) {
		short x1 = self.position.X; short y1 = self.position.Y;
		short x2 = o.position.X; short y2 = o.position.Y;
		return (x1 > x2);
	}

	int MoveAndCollide(short _dir) {
		if (!CanMove(position, _dir, speed)) { life = false; DeleteSelf(); ClearSelf(); return 0; }
		ClearSelf();
		switch (_dir) {
			case 0: {
				position.Y -= speed;
				break;
			}
			case 1: {
				position.X += speed;
				break;
			}
			case 2: {
				position.Y += speed;
				break;
			}
			case 3: {
				position.X -= speed;
				break;
			}
		}
		Update();
		return 0;
	}

	void SaveMemory() {
		ChangeCursorPosition(position.X, position.Y);
		for (int i = 0; i < (int)size(memory[0]); i++) {
			for (int j = 0; j < (int)size(memory[0]); j++) {
				//if (map[cursor_position.X + j][cursor_position.Y + i] != '_' && cursor_position.X == position.X + j && cursor_position.Y == position.Y + i) {
					//continue;
				//}
				memory[i][j] = map[cursor_position.Y + i][cursor_position.X + j];
			}
		}
	}

	void Update() {
		SaveMemory();
		ChangeCursorPosition(position.X, position.Y);
		for (int i = 0; i < (int)size(body); i++) {
			for (int j = 0; j < (int)size(body[0]); j++) {
				if (body[i][j] == ' ') {
					ChangeCursorPosition(position.X + j + 1, position.Y + i);
				}
				else {
					ChangeCursorPosition(position.X + j, position.Y + i);
					SetWriteColor(Green);
					cout << body[i][j];
				}
			}
		}
		SetWriteColor(Grey);
	}

	void ClearSelf() {
		ChangeCursorPosition(position.X, position.Y);
		for (int i = 0; i < (int)size(body); i++) {
			for (int j = 0; j < (int)size(body[0]); j++) {
				ChangeCursorPosition(position.X + j, position.Y + i);
				cout << memory[i][j];
			}
		}
	}

	void DeleteSelf() {
		for (int i = 0; i < (int)size(body); i++) {
			for (int j = 0; j < (int)size(body[0]); j++) {
				body[i][j] = ' ';
			}
		}
	}
};

Market market(27, 1);

void CreateMarket() {
	ChangeCursorPosition(market.position.X, market.position.Y);
	SetWriteColor(White);
	for (int i = 0; i < (int)size(market.body); i++) {
		for (int j = 0; j < (int)size(market.body[0]); j++) {
			ChangeCursorPosition(market.position.X + j, market.position.Y + i);
			cout << '\b';
			cout << market.body[i][j];	
			map[cursor_position.Y][cursor_position.X - 1] = market.body[i][j];
		}
	}
	SetWriteColor(Grey);
}

void CreateHuman(short x, short y, short dir) {
	Human h(x, y, dir);
}

void FillMap() {
	SetWriteColor(Grey);
	for (int i = 0; i < rows - 1; i++) {
		for (int j = 0; j < cols; j++) {
			cout << '_';
			map[i][j] = '_';
			colors_map[i][j].push_back(Grey);
		}
	}
}

void SpawnPeople() {
	short x;
	short y = GetRandomNumber(8, rows - 5);
	short dir = 1;
	short side = GetRandomNumber(0, 1);
	if (side == 0) { x = 0; }
	else { x = cols - 3; dir = 3; }
	CreateHuman(x, y, dir);
}

void InitComponents() {
	setlocale(LC_ALL, "Russian");
	//system("cls");
	system("mode con cols=60 lines=23");
	srand((unsigned)time(NULL));
	SetWindowPos(window_header, HWND_TOP, 30, 140, 600, 600, NULL);
}

void ShowHideCursor(bool flag) {
	ChangeCursorPosition(0, 0);
	GetConsoleCursorInfo(hConsole, &structCursorInfo);
	if (flag) { structCursorInfo.bVisible = TRUE; }
	else { structCursorInfo.bVisible = FALSE; }
	SetConsoleCursorInfo(hConsole, &structCursorInfo);
}

int main()
{
	InitComponents();

	FillMap();	

	CreateMarket();

	//SpawnPeople();

	CreateHuman(cols-3, 6, 3);

	//CreateHuman(0, 14, 1);

	ShowHideCursor(false);

	while (true) {

	}
}

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
