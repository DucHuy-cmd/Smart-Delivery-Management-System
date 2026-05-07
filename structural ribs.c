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
//x, y: toa do giao hang
typedef struct{
	int day;
	int month;
	int year;
}DATE;
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
	DATE date;
	struct ORDER *next;
}order;
typedef struct SHIPPER{
	char code[5];
	char Name[30];
	long long CCCD;
	int x;
	int y;
	double weight;
	int status;
	int numberOrder;
	struct SHIPPER *next;
}shipper;
#define PHI_SHIP 10000
#define PHI_TAI_TRONG 5000;
//Nhap vao so hang moi
void addOrder(order **headO){
    int number;
    drawHeader("ADD NEW ORDERS");
    printf("\n  [?] How many orders to add: ");
    if (scanf("%d", &number) != 1) return;

    order *tail = *headO;
    if(tail != NULL){
        while(tail->next != NULL) tail = tail->next;
    }

    for(int i = 0; i < number; i++){
        char title[64];
        sprintf(title, "INPUT ORDER %d / %d", i+1, number);
        drawHeader(title);

        order *newNode = (order*)malloc(sizeof(order));
        if(newNode == NULL) return;

        printf("  \xAF Enter order code: ");
        scanf("%s", newNode->code);
        getchar();

        printf("  \xAF Customer name: ");
        fgets(newNode->customerName, sizeof(newNode->customerName), stdin);
        newNode->customerName[strcspn(newNode->customerName, "\n")] = 0;

        printf("  \xAF Product name: ");
        fgets(newNode->orderName, sizeof(newNode->orderName), stdin);
        newNode->orderName[strcspn(newNode->orderName, "\n")] = 0;

        printf("  \xAF Delivery coordinates (x y): ");
        scanf("%d%d", &newNode->x, &newNode->y);

        printf("  \xAF Weight (kg): ");
        scanf("%lf", &newNode->weight);

        newNode->isSaved = 0;
        do {
            printf("  \xAF Priority (0: Normal, 1: Express): ");
            scanf("%d", &newNode->priority);
        } while(newNode->priority != 0 && newNode->priority != 1);

        newNode->fee = PHI_SHIP + newNode->weight * PHI_TAI_TRONG;
        if(newNode->priority == 1) newNode->fee *= 1.2;

        do {
            printf("  \xAF Status (0: Pending, 1: Shipping, 2: Delivered): ");
            scanf("%d", &newNode->status);
        } while(newNode->status < 0 || newNode->status > 2);

        printf("  \xAF Order date (dd mm yyyy): ");
        scanf("%d%d%d", &newNode->date.day, &newNode->date.month, &newNode->date.year);

        newNode->next = NULL;
        if(*headO == NULL){
            *headO = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    printf("\n  ------------------------------------------\n");
    printf("  [!] SUCCESS: %d ORDERS ADDED.\n", number);
}
//sap xep nam, thang, ngay
int compareDate(DATE d1, DATE d2){
	if(d1.year != d2.year) return d1.year - d2.year;
	else if(d1.month != d2.month) return d1.month - d2.month;
	return d1.day - d2.day;
}
//Sap xep danh sach don theo ngay giao hang
void SortOrder(order **head){
	if(*head == NULL) return;
	for(order *i = *head; i->next != NULL; i = i->next){
		for(order *j = i->next; j != NULL; j = j->next){
			if(compareDate(i->date, j->date)>0){
				order *nextI = i->next;//luu lai con bien i dang tro toi dau
				order *nextJ = j->next;
				//swap
				order temp = *i;
				*i = *j;
				*j = temp;
				//tra lai vi tri tro that su
				i->next = nextI;
				j->next = nextJ;
			}	
		}
	}
	printf("\n======SUCCESS SORT======\n");
}
void FindOrder(order **head){
	char codeFind[5];
	printf("Input code find: ");
	scanf("%s", codeFind);
	order *temp = *head;
	while(temp != NULL){
		if(strcmp(temp->code, codeFind) == 0){
			printf("%-30s || %-30s || %-10s || %-10s\n", "CustomerName", "OrderName", "Weight", "Date");
            printf("%-30s || %-30s || %-10.2f || %02d/%02d/%04d\n", temp->customerName, temp->orderName, temp->weight, temp->date.day, temp->date.month, temp->date.year);
            return;
		}
		temp = temp->next;
	}
	printf("No find order\n");
}
//Cap nhat don hang vao file
void SaveOrder(order **headO){
	FILE *f;
	f = fopen("Order_Information.txt", "a");
	if(f==NULL){
		printf("Error!");
		exit(0);
	}
	char *Status[]={"Pending", "Shipping", "Delivered"};
	//kiem tra file co dang trong ko
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	if(size==0){
		fprintf(f, "%-5s || %-20s || %-20s || %-3s || %-3s || %-6s || %-10s || %-10s || %-10s || %-10s\n",
				"Code", "Order Name", "Customer Name", "X", "Y", "Weight", "Date", "Fee", "Priority", "Status");
	}
	int count = 0;
	order *temp = *headO;
	if(temp==NULL){
		printf("\n===No orders have been entered into the file\n");
		return;
	}
	while(temp != NULL){
		if(temp->isSaved==0){
			fprintf(f, "%-5s || %-20s || %-20s || %-3d || %-3d || %-6.2lf || %02d/%02d/%04d || %-10.2lf || %-10s || %-10s\n"
					,temp->code, temp->orderName, temp->customerName, temp->x, temp->y, temp->weight, temp->date.day, 
					temp->date.month, temp->date.year,temp->fee, (temp->priority==1) ? "Express" : "Normal", Status[temp->status]);
			temp->isSaved = 1;
			++count;
		}
		temp = temp->next;
	}
	fclose(f);
	system("start Order_Information.txt");
	if(count!=0)
	printf("\n===There are %d orders in the file===\n", count);//Co %d don hang duoc nhap vao file
	else printf("\n===No orders have been entered into the file\n");//Khong co don hang nao duoc nhap vao file
}
void UpdateOrder(order **head){
    int number;
    drawHeader("UPDATE ORDER INFO");
    printf("\n  [?] Enter the order index to edit (1, 2, ...): ");
    if (scanf("%d", &number) != 1 || number < 1) return;

    order *temp = *head;
    while(number > 1 && temp != NULL) {
        temp = temp->next;
        number--;
    }

    if(temp == NULL){
        printf("\n  [x] Error: Order index not found!\n");
        return;
    }

    while(1){
        drawHeader("EDITING ORDER");
        printf("\n  [!] ORDER CODE:  %s\n", temp->code);
        printf("  ------------------------------------------\n");
        printf("  1. Customer name\n"
               "  2. Product name\n"
               "  3. Delivery address (x, y)\n"
               "  4. Freight volume\n"
               "  5. Priority level (0-1)\n"
               "  6. Order status (0-2)\n"
               "  7. Delivery time\n"
               "  0. EXIT UPDATE\n\n");

        int n;
        printf("  [?] Choose field (0-7): ");
        scanf("%d", &n);
        if(n == 0) break;

        printf("\n");
        switch(n){
            case 1:
                printf("  \xAF New customer name: ");
                getchar();
                fgets(temp->customerName, sizeof(temp->customerName), stdin);
                temp->customerName[strcspn(temp->customerName, "\n")] = 0;
                break;
            case 2:
                printf("  \xAF New product name: ");
                getchar();
                fgets(temp->orderName, sizeof(temp->orderName), stdin);
                temp->orderName[strcspn(temp->orderName, "\n")] = 0;
                break;
            case 3:
                printf("  \xAF New address (x y): ");
                scanf("%d %d", &temp->x, &temp->y);
                break;
            case 4:
                printf("  \xAF New weight: ");
                scanf("%lf", &temp->weight);
                break;
            case 5:
                do {
                    printf("  \xAF New priority (0-1): ");
                    scanf("%d", &temp->priority);
                } while(temp->priority != 0 && temp->priority != 1);
                break;
            case 6:
                do {
                    printf("  \xAF New status (0-2): ");
                    scanf("%d", &temp->status);
                } while(temp->status < 0 || temp->status > 2);
                break;
            case 7:
                printf("  \xAF New delivery time (dd mm yyyy): ");
                scanf("%d%d%d", &temp->date.day, &temp->date.month, &temp->date.year);
                break;
            default:
                printf("  Invalid selection!\n");
                break;
        }
        printf("\n  => Update successful!\n");
        printf("  Press any key to continue editing...");
        getch();
    }
}
// Ham ve giao dien Menu Quan ly don hang
void Draw_OrderMenu(int pointer){
    char *options[] = {
        "Add a new order",
        "Sort orders by date",
        "Find an order",
        "Update order info",
        "BACK TO MAIN MENU"
    };
    char buf[MENU_W + 2];
    drawHLine(0);
    drawTitleRow("[ 1 ]   ORDER MANAGEMENT");
    drawHLine(1);
    drawEmptyRow();
    for (int i = 0; i < 5; i++) {
        if (i == pointer)
            sprintf(buf, "  > [ %d ]. %s", (i == 4 ? 0 : i + 1), options[i]);
        else
            sprintf(buf, "    [ %d ]. %s", (i == 4 ? 0 : i + 1), options[i]);
        drawItemRow(buf, i == pointer);
    }
    drawEmptyRow();
    drawHLine(1);
    drawHintRow("    [^][v] Navigate   |   [ENTER] Select");
    drawHLine(2);
}
//OPTION 1
int Order_Management(order **headO, shipper **headS) {
    int pointer = 0;
    char key;
    
    while(1) {
        system("cls");
        Draw_OrderMenu(pointer);
        
        key = getch();

        // Xu lý phím mui tên
        if (key == -32) {
            key = getch();
            if (key == 72) { // Lên
                if (pointer > 0) pointer--;
                else pointer = 4;
            } else if (key == 80) { // Xuong
                if (pointer < 4) pointer++;
                else pointer = 0;
            }
        } 
        // Xu lý phím Enter
        else if (key == 13) {
            system("cls");
            if (pointer == 4) return 0; // Thoát ve Menu chính

            // Gui các hàm xu lý thuc thi
            switch (pointer) {
                case 0: 
                    addOrder(headO); 
                    break;
                case 1: 
                    SortOrder(headO); 
                    break;
                case 2: 
                    FindOrder(headO); 
                    break;
                case 3: 
                    UpdateOrder(headO); 
                    break;
            }

            // Sau khi thuc hien xong, dung lai xem ket qua
            printf("\n------------------------------------------\n");
            printf("Action completed. Press any key to return.");
            getch();
        }
    }
}
//OPTION 2
int Shipper_Management(order **headO, shipper **headS){
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Register as a new Shipper"
			"\n2. Update Location/Status (Available/Busy/On Break)"
			"\n3. View the list of active shippers\n\n");
	do{
		if(countChoice==3){
			return -1;
		}		
		printf("Enter your choice(1-3): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>3 || choiceTwo<1);
}
//OPTION 3
int Smart_Coordination(order **headO, shipper **headS){
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Find the nearest shipper"
			"\n2. Automatically assign orders by priority"
			"\n3. Suggestions for the optimal route for a delivery driver\n\n");
	do{
		if(countChoice==3){
			return -1;
		}		
		printf("Enter your choice(1-3): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>3 || choiceTwo<1);
}
//OPTION 4
int Statistics_and_Reports(order **headO, shipper **headS){
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Calculate the total daily revenue"
			"\n2. Statistics on the number of completed orders for each shipper"
			"\n3. Export the report\n\n");
	do{
		if(countChoice==3){
			return -1;
			}		
		printf("Enter your choice(1-3): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>3 || choiceTwo<1);		
}
//Mac dinh load don hang khi mo chuong trinh
void loadFileOrder(order **head){
	FILE *f = fopen("Order_Information.txt", "r");
	if(f==NULL) return;
	char header[500];
	//chi co dong tieu de
	if(fgets(header, sizeof(header), f)==NULL){
		fclose(f);
		return;
	}
	order *tail = *head;
	if(tail != NULL){
		while(tail->next != NULL)
		tail = tail->next;
	}
	//chay toi khi chua gap ket thuc file
	while(!feof(f)){
		char ptr[15];//bien luu gia tri cua priority
		char ptr2[20];//bien luu gia tri status
		order *newNode = (order*)malloc(sizeof(order));
		if(newNode==NULL) return;
		//%[^|] doc 1 chuoi co khoang trang cho den khi gap | thi dung lai
		if(fscanf(f, "%s || %[^|] || %[^|] || %d || %d || %lf || %d/%d/%d || %lf || %s || %s\n", 
			newNode->code, newNode->orderName, newNode->customerName, &newNode->x, &newNode->y, &newNode->weight,
			&newNode->date.day, &newNode->date.month, &newNode->date.year, &newNode->fee, ptr, ptr2)>0){
			if(strcmp(ptr, "Express") == 0) newNode->priority = 1;
			else newNode->priority = 0;
			if(strcmp(ptr2, "Pending") == 0) newNode->status = 0;
			else if(strcmp(ptr2, "Shipping") == 0) newNode->status = 1;
			else newNode->status = 2;
			newNode->next = NULL;
			newNode->isSaved = 0;
			if(*head == NULL){
				*head = newNode;
				tail = newNode;
			}
			else{
				tail->next = newNode;
				tail = newNode;
			}
		}
		else{
			free(newNode);//khong doc duoc thi giai phong bo nho
		}
	}
	fclose(f);
	printf("======DONE LOAD FILE ORDER=====\n");
}
//ham xoa khoang trang trong chu
void rtrim(char *str){
    if (str == NULL) return;
    int len = strlen(str);
    //isspace: neu con khoang trang thi tiep tuc ktra
    //str[len-1] vi ki tu cuoi cung la n-1
    while(len > 0 && isspace((unsigned char)str[len - 1])){
        len--;
    }
    str[len] = '\0';
}
//clear file de khi sap xep xong in lai se khong bi loi
void clear_file(){
	FILE *f = fopen("Order_Information.txt", "w");
	if(f != NULL){
		fclose(f);
	}
}
//to mau
void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

// Hàm an con tro 
void hideCursor(){
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
void Option(int pointer){
    char *menuItems[] = {
        "ORDER MANAGEMENT",
        "SHIPPER MANAGEMENT",
        "SMART COORDINATION",
        "STATISTICS AND REPORTS",
        "FREE MEMORY AND EXIT PROGRAM"
    };
    char buf[MENU_W + 2];
    int i;
    drawHLine(0);
    drawEmptyRow();
    drawTitleRow("\xDB\xDB\xDB  SMART DELIVERY MANAGEMENT  \xDB\xDB\xDB");
    drawTitleRow("Logistics & Smart Coordination System");
    drawTitleRow("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    drawEmptyRow();
    drawHLine(1);
    drawTitleRow("-=   M A I N   M E N U   =-");
    drawHLine(1);
    drawEmptyRow();
    for (i = 0; i < 5; i++) {
        if (i == pointer)
            sprintf(buf, "  > [ %d ]. %s", (i == 4 ? 0 : i + 1), menuItems[i]);
        else
            sprintf(buf, "    [ %d ]. %s", (i == 4 ? 0 : i + 1), menuItems[i]);
        drawItemRow(buf, i == pointer);
    }
    drawEmptyRow();
    drawHLine(1);
    drawHintRow("    [^][v] Navigate   |   [ENTER] Select");
    drawHLine(2);
}
//giai phong bo nho order
void freeOrder(order **headO){
	order *temp;
	while(*headO != NULL){
		temp = *headO;
		*headO = (*headO)->next;
		free(temp);
	}
}
//giai phong bo nho shipper
void freeShipper(shipper **headS){
	shipper *temp;
	while(*headS != NULL){
		temp = *headS;
		*headS = (*headS)->next;
		free(temp);
	}
}
// Menu chinh - sau moi chuc nang can nhan phim bat ky moi quay lai
void SelectOption(order **headO, shipper **headS) {
    loadFileShipper(headS);
    loadFileOrder(headO);
    hideCursor(); // an con tro chuot

    int pointer = 0; // Vi trï¿½ thanh sï¿½ng (0-4)
    char key;
    int (*Select[4])(order**, shipper**) = {Order_Management, Shipper_Management, Smart_Coordination, Statistics_and_Reports};

    system("cls");
    while(1) {
        goHome();
        Option(pointer);

        key = getch();//doi nhan phim

        if (key == -32) { // Phï¿½m mui tï¿½n lï¿½ phï¿½m mo rong (phim chuan bi nhap la phim dac biet)
            key = getch();
            if (key == 72) { // Mui tï¿½n lï¿½n
                if (pointer > 0) pointer--;
                else pointer = 4;
            } else if (key == 80) { // Mui tï¿½n xuong
                if (pointer < 4) pointer++;
                else pointer = 0;
            }
        } else if (key == 13) { // Phï¿½m ENTER
            if (pointer == 4) { // Lua chon thoï¿½t (0)
                clear_file();
                SaveOrder(headO);
                freeOrder(headO);
                freeShipper(headS);
                printf("\n====SUCCESS FREE MEMORY====\n");
                printf("\n====EXIT PROGRAM...GOOD BYE!====");
                return;
            } else {
                // Thuc hien chuc nang tuong ung
                system("cls");
                int check = Select[pointer](headO, headS);
                if (check == -1) {
                    printf("\n===SYSTEM LOCKED!===\n");
                    return;
                }
                system("cls"); // Xoa man hinh sau khi thuc hien xong chuc nang
            }
        }
    }
}
int main(){
	order *headOrder = NULL;
	shipper *headShipper = NULL;
	SelectOption(&headOrder, &headShipper);
}
