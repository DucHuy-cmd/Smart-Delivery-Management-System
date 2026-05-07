// Lib.h
#ifndef LIB_H
#define LIB_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<conio.h>
#include<ctype.h>
#include<time.h>
#include<stdbool.h>
#include<windows.h>
#include<conio.h>
typedef struct{
	int day;
	int month;
	int year;
}DATEE;

typedef struct ORDER{
	char code[5];
	char customerName[30];
	char orderName[30];
	int x;
	int y;
	double weight;
	int priority;
	double fee;
	int status;
	int isSaved; // 0: chua luu, 1: da duoc luu
	DATEE date;
	struct ORDER *next;
}order;

typedef struct SHIPPER{
	char code[5];
	char Name[30];
	long long CCCD;
	int x;
	int y;
	int prioritySP; // 0: shipper binh thuong, 1: shipper hoa toc
	double weight;
	int status;
	int numberOrder;
	int isSaved;
	struct SHIPPER *next;
}shipper;

typedef struct{ int x, y; } MapPoint;

#define PHI_SHIP 10000
#define PHI_TAI_TRONG 5000;
#define MAP_SIZE 30
#define MENU_W 55  // inner width of all menu box frames

char grid[MAP_SIZE][MAP_SIZE];
MapPoint parent[MAP_SIZE][MAP_SIZE];
bool visited[MAP_SIZE][MAP_SIZE];

static int N0S = 1;

// ---- OPTION 1: Order Management ----
int Order_Management(order **headO, shipper **headS);
void Draw_OrderMenu(int pointer);
void addOrder(order **headO);
void SaveOrder(order **headO);
int compareDate(DATEE d1, DATEE d2);
void FindOrder(order **head);
void SortOrder(order **head);
void UpdateOrder(order **head);

// ---- OPTION 2: Shipper Management ----
int Shipper_Management(order **headO, shipper **headS);
void Draw_ShipperMenu(int pointer);
void registerShipper (shipper **headS);
void deleteShipper (shipper **headS);
void informationShipper (shipper **headS);

// ---- OPTION 3: Smart Coordination ----
int Smart_Coordination(order **headO, shipper **headS);
void Draw_CoordinationMenu(int pointer);
void dispatchOrders(order **headO, shipper **headS);      // Dieu phoi tu dong
void suggestOptimalRoute(order **headO, shipper **headS); // Goi y duong di (ban do + BFS)
void warehouseOverview(order **headO, shipper **headS);   // Tong quan kho hang
void animateDelivery(order **headO, shipper **headS);

// ---- OPTION 4: Statistics and Reports ----
int Statistics_and_Reports(order **headO, shipper **headS);
void Draw_StatisticsMenu(int pointer);
double totalShip(order *headO);
void compileReport(order **headO, shipper **headS, char file[]);

// ---- Utility / File ----
void setColor(int bg, int fg);
void loadFileShipper(shipper **headS);
void loadFileOrder(order **head);
void rtrim(char *str);
void clear_file();

// ---- Main Selection ----
void SelectOption(order **headO, shipper **headS);
void Option(int pointer);


// ---- UI Box Drawing Helpers (CP437 box chars) ----
void goHome(void);
void drawHeader(const char *text);
void drawSubHeader(const char *text);
void drawHLine(int type);
void drawEmptyRow(void);
void drawTitleRow(const char *title);
void drawItemRow(const char *text, int highlighted);
void drawHintRow(const char *hint);
#endif
