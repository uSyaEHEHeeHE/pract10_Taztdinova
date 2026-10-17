#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>

using namespace std;

const int width = 30;
const int height = 15;

struct Segment {
    int x;
    int y;
    Segment* next;
};

struct Record {
    char name[20];
    int score;
};

Segment* head = nullptr;
int score = 0;
int direction = 1;
int foodX = 0;
int foodY = 0;
int length = 0;

void push_front(int x, int y) {
    Segment* n = new Segment{ x, y, head };
    head = n;
    length++;
}

void push_back(int x, int y) {
    Segment* n = new Segment{ x, y, nullptr };
    if (!head) {
        head = n;
    }
    else {
        Segment* p = head;
        while (p->next) {
            p = p->next;
        }
        p->next = n;
    }
    length++;
}

void pop_back() {
    if (!head) {
        return;
    }
    if (!head->next) {
        delete head;
        head = nullptr;
        length = 0;
        return;
    }
    Segment* p = head;
    while (p->next->next) {
        p = p->next;
    }
    delete p->next;
    p->next = nullptr;
    length--;
}

void clear() {
    while (head) {
        pop_back();
    }
}

bool on_snake(int x, int y, bool skipTail) {
    for (Segment* p = head; p; p = p->next) {
        if (p->x == x && p->y == y) {
            if (skipTail && !p->next) {
                return false;
            }
            return true;
        }
    }
    return false;
}

void spawn_food() {
    do {
        foodX = rand() % width;
        foodY = rand() % height;
    } while (on_snake(foodX, foodY, false));
}

void new_game() {
    clear();
    score = 0;
    direction = 1;

    int sx = width / 2;
    int sy = height / 2;

    push_front(sx, sy);
    push_back(sx - 1, sy);
    push_back(sx - 2, sy);

    spawn_food();
}

void draw() {
    system("cls");

    for (int i = 0; i < width + 2; i++) {
        cout << "#";
    }
    cout << endl;

    for (int y = 0; y < height; y++) {
        cout << "#";
        for (int x = 0; x < width; x++) {
            if (head && head->x == x && head->y == y) {
                cout << "O";
            }
            else if (on_snake(x, y, false)) {
                cout << "o";
            }
            else if (x == foodX && y == foodY) {
                cout << "*";
            }
            else {
                cout << " ";
            }
        }
        cout << "#" << endl;
    }

    for (int i = 0; i < width + 2; i++) {
        cout << "#";
    }
    cout << endl;

    cout << "Счет: " << score << "  Длина: " << length << endl;
    cout << "WASD - управление, P - сохранить" << endl;
}

bool move_snake() {
    int nx = head->x;
    int ny = head->y;

    if (direction == 0) {
        ny--;
    }
    if (direction == 1) {
        nx++;
    }
    if (direction == 2) {
        ny++;
    }
    if (direction == 3) {
        nx--;
    }

    if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        return false;
    }

    bool eat = (nx == foodX && ny == foodY);

    if (on_snake(nx, ny, !eat)) {
        return false;
    }

    push_front(nx, ny);
    if (eat) {
        score = score + 10;
        spawn_food();
    }
    else {
        pop_back();
    }
    return true;
}

void save_game() {
    ofstream file("save.bin", ios::binary);
    if (!file.is_open()) {
        cout << "Не удалось сохранить игру" << endl;
        return;
    }

    file.write((char*)&score, sizeof(score));
    file.write((char*)&direction, sizeof(direction));
    file.write((char*)&foodX, sizeof(foodX));
    file.write((char*)&foodY, sizeof(foodY));
    file.write((char*)&length, sizeof(length));

    for (Segment* p = head; p; p = p->next) {
        file.write((char*)&p->x, sizeof(p->x));
        file.write((char*)&p->y, sizeof(p->y));
    }

    file.close();
}

bool load_game() {
    ifstream file("save.bin", ios::binary);
    if (!file.is_open()) {
        return false;
    }

    clear();

    int savedLength = 0;
    file.read((char*)&score, sizeof(score));
    file.read((char*)&direction, sizeof(direction));
    file.read((char*)&foodX, sizeof(foodX));
    file.read((char*)&foodY, sizeof(foodY));
    file.read((char*)&savedLength, sizeof(savedLength));

    for (int i = 0; i < savedLength; i++) {
        int x;
        int y;
        file.read((char*)&x, sizeof(x));
        file.read((char*)&y, sizeof(y));
        push_back(x, y);
    }

    file.close();
    return true;
}

void add_record(int finalScore) {
    Record r;
    cout << "Введите имя (без пробелов): ";
    cin >> r.name;
    r.score = finalScore;

    ofstream file("records.bin", ios::binary | ios::app);
    if (!file.is_open()) {
        cout << "Не удалось открыть файл рекордов" << endl;
        return;
    }
    file.write((char*)&r, sizeof(r));
    file.close();
}

void show_records() {
    ifstream file("records.bin", ios::binary);
    if (!file.is_open()) {
        cout << "Рекордов пока нет" << endl;
        return;
    }

    cout << "Таблица рекордов" << endl;

    Record r;
    int count = 0;
    while (count < 100 && file.read((char*)&r, sizeof(Record))) {
        count++;
        cout << count << ". " << r.name << " - " << r.score << endl;
    }
    file.close();

}

void play_game() {
    bool running = true;

    while (running) {
        draw();

        if (_kbhit()) {
            char key = _getch();

            if ((key == 'w' || key == 'W') && direction != 2) {
                direction = 0;
            }
            if ((key == 'd' || key == 'D') && direction != 3) {
                direction = 1;
            }
            if ((key == 's' || key == 'S') && direction != 0) {
                direction = 2;
            }
            if ((key == 'a' || key == 'A') && direction != 1) {
                direction = 3;
            }
            if (key == 'p' || key == 'P') {
                save_game();
                cout << "Игра сохранена" << endl;
                Sleep(5000);
            }
        }

        if (!move_snake()) {
            running = false;
        }

        Sleep(150);
    }

    system("cls");
    cout << "Игра окончена! Ваш счет: " << score << endl;
    add_record(score);
    clear();
}

int main()
{
    setlocale(NULL, "Russian");
    srand(time(0));

    int choice = 0;

    do {
        system("cls");
        cout << "\n1. Новая игра\n";
        cout << "\n2. Загрузить игру\n";
        cout << "\n3. Таблица рекордов\n";
        cout << "\n4. Выход\n" << endl;
        cout << "Ваш выбор: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            new_game();
            play_game();
            break;
        }
        case 2: {
            if (load_game()) {
                play_game();
            }
            else {
                cout << "Сохранение не найдено\n";
                Sleep(1500);
            }
            break;
        }
        case 3: {
            system("cls");
            show_records();
            cout << "Нажмите любую клавишу...\n";
            _getch();
            break;
        }
        case 4: {
            cout << "Выход...\n";
            break;
        }
        default:
            cout << "Неверный пункт.\n";
            break;
        }
    } while (choice != 4);

    clear();
    return 0;
}