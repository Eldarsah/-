#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <Windows.h>



using namespace std;


struct Point {
	int i;
	int j;

	Point() {}
	Point(int i, int j) {
		this->i = i;
		this->j = j;
	}
};

enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};



HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void SetColor(ConsoleColor text, ConsoleColor background) {
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((background << 4) | text));
}

int main() {
	// Этап 1. Считывание лабиринта из файла

	fstream input("input.txt", fstream::in);

	Point start_point, end_point, boundary_point;

	// Массив массивов для хранения карты лабиринта
	vector<vector<int>> map;
	// Объявление массива для добавления в последующем очередной считываемой из файла строки
	vector<int> row;
	int row_len;
	string line;
	int i = 0, j;

	// Пробегаемся по всем строкам в файле
	while (getline(input, line)) {
		// Меняем размер массива на длину считанной строки
		row.resize(line.length());

		// Пробегаемся по считанной строке
		for (j = 0; j < line.length(); j++) {
			// Так как элементы файла это char символы, то чтобы получить численный вариант отображенный в файле нужно вычесть число эквивалентное символу '0'
			row[j] = line[j] - '0';

			// Если наткнулись на двойку, то это значит, что мы попали на стартовую позицию, что мы и фиксируем
			// Само значение на карте заменяем для дальнейшей правильной обработки
			if (row[j] == 2) {
				start_point = Point(i, j);
				row[j] = 1;///////////////////////////////////////////////////////////////////////////
			}
			// Если наткнулись на тройку, то это значит, что мы попали на конечную позицию, что мы и фиксируем
			// Само значение на карте заменяем для дальнейшей правильной обработки
			else if (row[j] == 3) {
				end_point = Point(i, j);
				row[j] = 1;
			}
		}

		i++;
		// Добавляем полученный вектор, являющийся строкой лабиринта в карту ( массив массивов)
		map.push_back(row);
	}

	// Инициализируем краевую точку для учёта в последующем границ лабиринта
	boundary_point.i = map.size() - 1;
	boundary_point.j = map[0].size() - 1;

	input.close();


	// Выводим получившийся считанный лабиринт
	for (int i = 0; i < map.size(); i++) {
		for (int j = 0; j < map[0].size(); j++) {
			if (map[i][j] == 0)
				SetColor(White, Red);
			else
				SetColor(Black, Green);

			cout << setw(4) << map[i][j];
		}
		cout << endl;
	}
	cout << endl;



	// Этап 2. Предварительное построение для поиска кратчайшего пути

	// Объявляем очередь непомеченных(свободных для заполнения) клеток, в которую изначально кладём только стартовую точку
	queue<Point> free_points;
	free_points.push(start_point);

	Point current;
	int d = 2;

	// Бежим пока очередь не иссякнет
	while (free_points.size() != 0) {

		// Вычисляем количество элементов в очереди(неразмеченных клеток)) и пробегаемся по всем ним
		int run_length = free_points.size();
		for (int k = 0; k < run_length; k++) {
			// Извлекаем очередной элемент
			current = free_points.front();
			free_points.pop();

			// Для удобства используем простые переменные вместо длинного названия точки
			i = current.i;
			j = current.j;

			// Поочерёдно проверяем горизонтальные и вертикальные соседние ячейки
			// Если координаты клетки не выходят за пределы лабиринта и она не помечена ( т.е. значения в ней 1 ), то мы помечаем её значением d
			// И кладём клетку с этими координатами в конец очереди
			if (j - 1 >= 0 && map[i][j - 1] == 1) {
				map[i][j - 1] = d;
				free_points.push(Point(i, j - 1));
			}
			if (j + 1 <= boundary_point.j && map[i][j + 1] == 1) {
				map[i][j + 1] = d;
				free_points.push(Point(i, j + 1));
			}
			if (i - 1 >= 0 && map[i - 1][j] == 1) {
				map[i - 1][j] = d;
				free_points.push(Point(i - 1, j));
			}
			if (i + 1 <= boundary_point.i && map[i + 1][j] == 1) {
				map[i + 1][j] = d;
				free_points.push(Point(i + 1, j));
			}
		}
		// Увеличиваем значение волны на 1
		d += 1;

	}

	// Значение стартовой точки на карте делаем равным 1 для правильного восстановления пути
	map[start_point.i][start_point.j] = 1;



	// Этап 3. Выявление кратчайшего пути

	vector<Point> path;
	i = end_point.i, j = end_point.j;
	// Точка с которой начинаем - конечная
	current = Point(end_point.i, end_point.j);
	// Создаём копию карты, чтобы на ней отобразить искомый в дальнейшем путь, чтобы было возможно отобразить лабиринт с путём в консоли
	vector<vector<int>> viewing_map(map);

	viewing_map[end_point.i][end_point.j] = d + 1;

	//Ищем путь. Поиск производится, начиная с конечной точки и до тех пор пока мы не натолкнёмся на значение начальной точки(её значение равно 1)
	while (map[current.i][current.j] != 1) {
		// Для очередной точки мы проверяем вокруг неё в соседних вертикальных и горизонтальных клетках значение на единицу меньшее
		// Проверяем не вышли ли мы за пределы поля и является ли значение в проверяемой клетке меньшим на единицу значений текущей клетки
		// Если да, то сдвигаемся в соответствующую сторону или по j для горизонтальных клеток, или по i для вертикальных
		if (current.j + 1 <= boundary_point.j && map[current.i][current.j + 1] == map[current.i][current.j] - 1) current.j += 1;
		else if (current.i + 1 <= boundary_point.i && map[current.i + 1][current.j] == map[current.i][current.j] - 1) current.i += 1;
		else if (current.i - 1 >= 0 && map[current.i - 1][current.j] == map[current.i][current.j] - 1) current.i -= 1;
		else if (current.j - 1 >= 0 && map[current.i][current.j - 1] == map[current.i][current.j] - 1) current.j -= 1;
		else
			cout << "Путь отсутствует!";

		path.push_back(current);
		viewing_map[current.i][current.j] = d + 1;
	}



	// Этап 4. Вывод лабиринта и кратчайшего пути на консоль

	// Проходимся по всей карте лабиринта и в зависимости от значений меняем цвет текста и фона
	for (int i = 0; i < map.size(); i++) {
		for (int j = 0; j < map[0].size(); j++) {
			if (viewing_map[i][j] == d + 1)
				SetColor(Black, Cyan);
			else if (viewing_map[i][j] == 0)
				SetColor(White, Red);
			else
				SetColor(Black, Green);

			cout << setw(4) << map[i][j];
		}
		cout << endl;
	}

	return 0;
}
