#include <windows.h>
#include <string.h>
#include <vector>
//------------------------------------------16 Вариант---------------------------------------------
const char windowClass[] = "win32app";
const char windowTitle[] = "Лабораторная работа номер 4. Графический постпроцессор.";
const char stringToDraw[] = "";
using namespace std;
int N;
vector<double> x;       //Вектор значений Х.
vector<double> yGRAF1;    //Вектор значений У для 1-го графика.
vector<double> yGRAF2;    //Вектор значений У для 2-го графика.
double maxX = 0, maxY = 0, minY = 1000000;
double kX = 0, kY = 0;
double xDL, yDL;

char name1[32], name2[32];
double val;

PAINTSTRUCT ps;
HDC hdc;
RECT rect;
HPEN pen, old_pen;
HBRUSH brush, old_brush;

void SETKA(HDC hdc, HWND hWnd)
{

	HBRUSH brush, old_brush;
	RECT rect;
	GetClientRect(hWnd, &rect);

	//Цвет области графиков - светло-серый
	brush = CreateSolidBrush(RGB(211, 211, 211));   
	old_brush = (HBRUSH)SelectObject(hdc, brush);

	//Создание окна. Отступ от краёв - 10 пикселей.
	Rectangle(hdc, rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 10);
	

	//Построение точек окна. Один пиксель.
	for (int i = 20; i<rect.right - 20; i += 10)
	{
		for (int j = 20; j<rect.bottom - 20; j += 10)
		{
			//Цвет сетки - чёрный.
			SetPixel(hdc, i, j, RGB(0, 0, 0));             
		}
	}

	//Удаление цвета заливки окна.
	SelectObject(hdc, old_brush);                 
	DeleteObject(brush);

};

void Fales(HDC hdc, HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	x.clear();       //очистка векторов
	yGRAF1.clear();
	yGRAF2.clear();

	// подключение файлов
	//FILE *f = fopen("1test.txt", "r");
	FILE *f = fopen("2sine_1pi.txt", "r");
	//FILE *f = fopen("3sine_5pi.txt", "r");
	//FILE *f = fopen("4rc.txt", "r");

	if (!f)
	{
		// тут пишем код, который выполняется, если файл открыть не удалось
		Rectangle(hdc, rect.left + 100, rect.top + 100, rect.right - 100, rect.bottom - 100);
		return;
	}

	fscanf(f, "%d", &N);

	if (N == 1)
	{
		fscanf(f, "%s", name1);
		while (!feof(f))
		{
			fscanf(f, "%lf", &val);
			if (feof(f))
				break;
			x.push_back(val);
			fscanf(f, "%lf", &val);
			yGRAF1.push_back(val);
		}                                                   
		//yGRAF1.pop_back();      //удаление последнего элемента
		//	x.pop_back();         //удаление последнего элемента
	}
	else
	{
		fscanf(f, "%s", name1);
		fscanf(f, "%s", name2);
		while (!feof(f))
		{
			fscanf(f, "%lf", &val);
			x.push_back(val);
			fscanf(f, "%lf", &val);
			yGRAF1.push_back(val);
			fscanf(f, "%lf", &val);
			yGRAF2.push_back(val);
		}
		yGRAF2.pop_back();  //удаление последнего элемента
		yGRAF1.pop_back();      //удаление последнего элемента
		x.pop_back();         //удаление последнего элемента

	}

	fclose(f);
	//x.pop_back();         //удаление последнего элемента
	//yGRAF1.pop_back();      //удаление последнего элемента
};
void calculator(HDC hdc, HWND hWnd)
{
	xDL = x.size();     //длина строки
	yDL = yGRAF1.size();  //длина строки

	for (int i = 0; i < xDL; i++)         //поиск максимального и минимального Х
	{
		if (x[i] > maxX)
			maxX = x[i];
	}

	if (N == 1)
	{
		for (int i = 0; i < yDL; i++)     //поиск максимального и минимального У
		{                                 //для случая с одним графиком 

			if (yGRAF1[i] > maxY)
				maxY = yGRAF1[i];
			if (yGRAF1[i] < minY)
				minY = yGRAF1[i];
		}
	}
	else                                  //поиск максимального и минимального У
	{									  //для случая с двумя графиком 
		for (double i = 0; i<yDL; i += 1)
		{
			if (yGRAF1[i] > maxY)
				maxY = yGRAF1[i];
			if (yGRAF2[i] > maxY)
				maxY = yGRAF2[i];
			if (yGRAF1[i] < minY)
				minY = yGRAF1[i];
			if (yGRAF2[i] < minY)
				minY = yGRAF2[i];
		}
	}

	kX = (rect.right - rect.left - 40) / maxX;            //коэффициенты Х и У
	kY = (rect.bottom - rect.top - 40) / (maxY - minY);

};
void charting(HDC hdc, HWND hWnd)
{


	calculator(hdc, hWnd);

	if (minY < 0)// построение графика при отрицательных значениях
	{
		pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));   //создание цвета ручки
		old_pen = (HPEN)SelectObject(hdc, pen);

		for (int i = 0; i < xDL - 1; i++)
		{
			MoveToEx(hdc, x[i] * kX + 20, rect.bottom - 20 - kY - yGRAF1[i] * kY, NULL);
			LineTo(hdc, x[i + 1] * kX + 20, rect.bottom - 20 - kY - yGRAF1[i + 1] * kY);
		}
		SelectObject(hdc, old_pen);                    //удаления цвета ручки
		DeleteObject(pen);
	}
	else // при положительных .
	{
		pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));  //создание цвета ручки
		old_pen = (HPEN)SelectObject(hdc, pen);
		for (int i = 0; i < xDL - 1; i++)//построение первого графика
		{
			MoveToEx(hdc, x[i] * kX + 20, rect.bottom - 20 - yGRAF1[i] * kY, NULL);
			LineTo(hdc, x[i + 1] * kX + 20, rect.bottom - 20 - yGRAF1[i + 1] * kY);
		}

		SelectObject(hdc, old_pen);                     //удаление цвета ручки
		DeleteObject(pen);

		if (N == 2) //построение второго графика
		{
			pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));//создание цвета ручки
			old_pen = (HPEN)SelectObject(hdc, pen);
			for (int i = 0; i < xDL - 1; i++)
			{
				MoveToEx(hdc, x[i] * kX + 20, rect.bottom - 20 - yGRAF2[i] * kY, NULL);
				LineTo(hdc, x[i + 1] * kX + 20, rect.bottom - 20 - yGRAF2[i + 1] * kY);
			}
			SelectObject(hdc, old_pen);                  //удаление цвета ручки
			DeleteObject(pen);
		}
	}
};
void yzli(HDC hdc, HWND hWnd)
{
	pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));      //создание цвета ручки
	old_pen = (HPEN)SelectObject(hdc, pen);
	brush = CreateSolidBrush(RGB(255, 0, 0));          //создание цвета заливки
	old_brush = (HBRUSH)SelectObject(hdc, brush);

	if (minY < 0)//рисуем точки на графиках
	{
		for (int i = 0; i < xDL; i++)
			Rectangle(hdc, x[i] * kX + 15, rect.bottom - 15 - kY - yGRAF1[i] * kY, x[i] * kX + 25, rect.bottom - 25 - kY - yGRAF1[i] * kY);
	}
	else
	{
		for (int i = 0; i < xDL; i++)
			Rectangle(hdc, x[i] * kX + 15, rect.bottom - 25 - yGRAF1[i] * kY, x[i] * kX + 25, rect.bottom - 15 - yGRAF1[i] * kY);
	}

	SelectObject(hdc, old_brush);                        //удаление цвета заливки
	DeleteObject(brush);

	if (N == 2)
	{
		brush = CreateSolidBrush(RGB(0, 255, 0));        //создание цвета заливки
		old_brush = (HBRUSH)SelectObject(hdc, brush);
		for (int i = 0; i < xDL; i++)
			Rectangle(hdc, x[i] * kX + 15, rect.bottom - 25 - yGRAF2[i] * kY, x[i] * kX + 25, rect.bottom - 15 - yGRAF2[i] * kY);
		SelectObject(hdc, old_brush);                    //удаление цвета заливки
		DeleteObject(brush);

	}
	SelectObject(hdc, old_pen);         //удаление цвета ручки
	DeleteObject(pen);
};




long __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	GetClientRect(hWnd, &rect);

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		SETKA(hdc, hWnd);
		Fales(hdc, hWnd);
		charting(hdc, hWnd);
		yzli(hdc, hWnd);

		TextOut(hdc, 450, 400, stringToDraw, strlen(stringToDraw));
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_LBUTTONDOWN:

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	return 0;
}


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, "Can’t register window class!", "Win32 API Test", NULL);
		return 1;
	}

	HWND hWnd = CreateWindow(windowClass, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 800, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, "Can’t create window!", "Win32 API Test", NULL);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
