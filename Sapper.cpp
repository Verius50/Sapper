#include <iostream>
#include "conio.h"      //для получения символа с клавиатуры
#include "windows.h"    //для изменения цвета и перевода курсора в консоли
#include <vector>           //динамический массив для рекурсии(будет дальше)

const int boardSizeX = 14, boardSizeY = 14, chance = 10;      //размеры поля и шанс на появление мины в конкретной клетке (1/chance)

struct coordinates
{
    int x, y;
};

void SetColor(int text, int background)             //устанавливает цвет и фон текста в консоли
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

std::vector<coordinates> checked0s;         //для рекурсивного открытия пустых клеток

void openAbout0(int* cells, int* cellsType, coordinates curs);      //экземпляр этой рекурсивной функции

class board
{
private:
    int cells[boardSizeY][boardSizeX];          // -1 - бомба, 0 - пустая ячейка, число 1-8 - кол-во бомб рядом
    int cellsType[boardSizeY][boardSizeX];      // 0 - ячейка закрыта, 1 - ячейка открыта, 2 - ячейка отмечена флажком
    coordinates cursor;             //курсор
    
public:
    board()
    {
        srand(time(0)); 
        for (int y = 0; y < boardSizeY; y++)
            for (int x = 0; x < boardSizeX; x++)
            {
                if (rand() % chance == 0)           //случайно расставляем мины
                    cells[y][x] = -1;           
                else cells[y][x] = 0;       
                cellsType[y][x] = 0;            //и закрываем все ячейки
            }
        cursor = {0, 0};
    }
    void openClose(int type){               //открывает/закрывает все ячейки
        for (int y = 0; y < boardSizeY; y++)
            for (int x = 0; x < boardSizeX; x++)
                cellsType[y][x] = type;
    }
    void countCellsValues()                    //считает кол-во бомб рядом для каждой клетки
    {
        for (int y = 0; y < boardSizeY; y++)        //перебираем все клетки на поле
            for (int x = 0; x < boardSizeX; x++)
                if (cells[y][x] == 0)               //если клетка пустая(не мина)
                    for (int i = -1; i <= 1; i++)       //то перебираем квадрат 3х3, где наша клетка в центре, а остальные рядом с ней
                        for (int j = -1; j <= 1; j++)       //тут i - y-координата этого квадрата, j - x-координата
                            if (x + j >= 0 && x + j < boardSizeX && y + i >= 0 && y + i < boardSizeY)       
                                //если итоговые координаты не за пределами поля
                                if (cells[y + i][x + j] == -1)      //и если в ячейке по ним находится мина
                                    cells[y][x]++;          //то у текущей ячейки(той, что в центре мнимого квадрата) увеличиваем значение
    }
    void show()        //отображение таблицы символами таблицы ascii
    {
        std::cout << char(201);     //╔
        for (int x = 0; x < boardSizeX; x++)
        {
            std::cout << char(205) << char(205) << char(205); //3 раза ═, потому что 1 клетка в ширину 3 символа 
            if (x < boardSizeX - 1)
                std::cout << char(203);         //и один раз ╦(для разделительной полоски), если это не самый последний столбец
        }
        std::cout << char(187) << '\n';         //╗


        for (int y = 0; y < boardSizeY; y++)
        {
            std::cout << char(186);     //левая граница ║
            for (int x = 0; x < boardSizeX; x++) 
            {
                std::cout << ' ';       //первый пробел ячейки (чтобы они были квадратными)
                if (cursor.x == x && cursor.y == y)
                    SetColor(13, 15);       //меняем цвет фона на белый, если курсор в этом месте
                if (cellsType[y][x] == 1)       //если ячейка открыта
                    if (cells[y][x] == -1)      // и если там бомба
                    {
                        SetColor(7, 4);
                        std::cout << '%';       //рисуем её
                    }
                    else if (cells[y][x] > 0)   //если она открыта и там число    
                    {
                        if (cursor.x == x && cursor.y == y)
                            SetColor(13, cells[y][x]);      //если курсор тут, то цвет текста делаем фиолетовым, а цвет фона по номеру ячейки
                        else SetColor(7, cells[y][x]);      //если нет, то цвет текста обычный, а цвет фона такой же
                        std::cout << cells[y][x];       //и выводим номер ячейки
                    }
                    else std::cout << ' ';      //второй пробел
                else {              //если ячейка закрыта
                    if(cellsType[y][x] == 2)        
                        SetColor(1, 14);    //если она отмечена флажком, то её фон жёлтый
                    std::cout << '#';       //рисуем её закрытой
                
                }SetColor(7, 0);        //меняем цвет на обычный

                std::cout << ' ' << char(186);      // правая граница ║
            }
            if (y < boardSizeY - 1)     // тут рисуем вот таукю горизонтальную перегородку ╠══╬══╬══╬══╣
            {
                std::cout << '\n' << char(204); //╠
                for (int x = 0; x < boardSizeX; x++) {
                    std::cout << char(205) << char(205) << char(205);       //═══
                    if (x < boardSizeX - 1) 
                        std::cout << char(206);         //╬
                }
                std::cout << char(185) << '\n';         //╣
            }
        }

        std::cout << '\n' << char(200);   //╚  
        for (int x = 0; x < boardSizeX; x++)        //нижняя граница
        {
            std::cout << char(205) << char(205) << char(205);   // ══
            if (x < boardSizeX - 1)
                std::cout << char(202);     //╩
        }   
        std::cout << char(188) << '\n';     //╝
    }
    bool move(int key)      //сдвинуть курсор или выволнить действие над ячейкой
    {                       //у каждой клавиши на клавиатуре есть свой код
        switch (key)
        {
        case 77:         //стрелка вправо
            if (cursor.x < boardSizeX - 1)          //соответствующим образом сдвигаем курсор
                cursor.x++;
            break;
        case 75:         //стрелка влево
            if (cursor.x > 0)          //соответствующим образом сдвигаем курсор
                cursor.x--;
            break;
        case 72:         //стрелка вверх
            if (cursor.y > 0)          //соответствующим образом сдвигаем курсор
                cursor.y--;
            break;
        case 80:         //стрелка вниз
            if(cursor.y < boardSizeY - 1)          //соответствующим образом сдвигаем курсор
                cursor.y++;
            break;
        case 13:         //enter
            cellsType[cursor.y][cursor.x] = 1;      //открываем ячейку
            if (cells[cursor.y][cursor.x] == 0)     //если она пустая
            {
                openAbout0(*cells, *cellsType,  cursor);        //запускаем рекурсию с этой точки, чтобы открыть все соседние пустые ячейки
                checked0s.push_back({ cursor.x, cursor.y });    //записываем эту ячейку в массив, чтобы знать, что её уже открыли
            }
            if (cells[cursor.y][cursor.x] == -1)        //если там бомба, что возвращаем проигрыш
                return 1;
            break;
        case 49:         //нажата кнопка 1        
            cellsType[cursor.y][cursor.x] = 2;          //отмечаем жёлтым соответствующую ячейку
            break;
        }
        return 0;
    }
    bool youWin()           //проверяем выйрал ли игрок
    {
        for (int y = 0; y < boardSizeY; y++)
            for (int x = 0; x < boardSizeX; x++)        //перебираем все ячейки
                if (cells[y][x] == -1 && cellsType[y][x] != 2)      //если хоть какая-то мина не отмечена
                    return 0;          //то это не победа
        return 1;       //иначе победа
    }
};

void openAbout0(int* cells, int* cellsType, coordinates curs)       //рекурсивно открываем пустые ячейки
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)       //перебираем мнимый квадрат 3х3, как уже было до этого
            if (curs.x + j >= 0 && curs.x + j < boardSizeX && curs.y + i >= 0 && curs.y + i < boardSizeY) //если он не выходит за границы поля
                if (*(cells + (curs.y + i) * boardSizeX + curs.x + j) == 0)     //по указателю получаем значение ячейки и убеждаемся,
                {                                                               //что оно равно нулю
                    bool checked = 0;       //теперь нужно проверить, не проверяли ли мы эту ячейку до этого 
                                            //чтобы не было бесконечного цикла и переполнения стека (stackoferflow)
                    for (int c = 0; c < checked0s.size(); c++)      //перебираем все проверенные ячейки
                        if (checked0s[c].y == curs.y + i && checked0s[c].x == curs.x + j)   //если там есть наша
                            checked = 1;    //то останавливаем дальнейшее открытие
                    if (!checked)       //если же эту ячейку ещё не проверяли
                    {
                        *(cellsType + (curs.y + i) * boardSizeX + curs.x + j) = 1;      //открываем её
                        checked0s.push_back({ curs.x + j, curs.y + i });        //и записываем её координаты,
                                                                                //чтобы знать потом, что её уже открывали
                        openAbout0(cells, cellsType, { curs.x + j, curs.y + i });   //и запускаем эту же рекурсивную функцию уже для 
                                                                                    //этой(только что открытой) клетки,
                                                                                    //чтобы проверить все клетки рядом с ней
                    }
                } else if(*(cells + (curs.y + i) * boardSizeX + curs.x + j) > 0)    //если у ожной из соседних ячеек номер не 0
                    *(cellsType + (curs.y + i) * boardSizeX + curs.x + j) = 1;      //мы её просто открываем
}

int main()
{
    board Board;
    Board.countCellsValues();
    Board.show();

    while (1)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);      //переводим курсор на начало
        SetConsoleCursorPosition(hConsole, { 0, 0 });           //переводим курсор на начало

        if(Board.move(_getch()))        //если функция действия вернула 1, значит мы открыли мину и проиграли
        {
            Board.openClose(1);     //открываем все ячейки
            Board.show();
            std::cout << "Game over!";
            break;
        }
        Board.show();       //если нет, то продолжаем
        if (Board.youWin()) //проверяем, не отметили ли мы уже все мины
        {
            Board.openClose(1);     //если да, то открываем все ячейки
            Board.show();
            std::cout << "You win!";        //победа
            break;
        }
    }
    system("pause");
    system("pause");    //не завершаем работу программы
    system("pause");
    system("pause");
    system("pause");

    return 0;
}

