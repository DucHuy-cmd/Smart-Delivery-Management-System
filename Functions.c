#include "Lib.h"
//to mau
void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

// Ham an con tro 
void hideCursor(){
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Move cursor to top-left without clearing (anti-flicker)
void goHome(void) {
    COORD _pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), _pos);
}

// ============================================================
//  UI BOX-DRAWING HELPERS  (Windows CP437 box characters)
//  Total frame width = MENU_W + 2 = 57 characters total
// ============================================================
void drawHLine(int type) {
    char L, R;
    if      (type == 0) { L = '\xC9'; R = '\xBB'; }
    else if (type == 1) { L = '\xCC'; R = '\xB9'; }
    else                { L = '\xC8'; R = '\xBC'; }
    setColor(11, 0);
    printf("%c", L);
    for (int i = 0; i < MENU_W; i++) printf("\xCD");
    printf("%c\n", R);
    setColor(15, 0);
}
void drawEmptyRow(void) {
    setColor(11, 0); printf("\xBA");
    setColor(15, 0); printf("%-*s", MENU_W, "");
    setColor(11, 0); printf("\xBA\n");
    setColor(15, 0);
}
void drawTitleRow(const char *title) {
    int len   = (int)strlen(title);
    int left  = (len < MENU_W) ? (MENU_W - len) / 2 : 0;
    int right = (len < MENU_W) ? MENU_W - len - left : 0;
    setColor(11, 0); printf("\xBA");
    setColor(14, 0);
    if (len >= MENU_W) printf("%-*.*s", MENU_W, MENU_W, title);
    else               printf("%*s%s%*s", left, "", title, right, "");
    setColor(11, 0); printf("\xBA\n");
    setColor(15, 0);
}
void drawItemRow(const char *text, int highlighted) {
    setColor(11, 0); printf("\xBA");
    if (highlighted) setColor(0, 11);
    else             setColor(15, 0);
    int len = (int)strlen(text);
    printf("%s", text);
    for(int i = len; i < MENU_W; i++) printf(" ");
    setColor(11, 0); printf("\xBA\n");
    setColor(15, 0);
}
void drawHintRow(const char *hint) {
    setColor(11, 0); printf("\xBA");
    setColor(8, 0);
    printf("%-*s", MENU_W, hint);
    setColor(11, 0); printf("\xBA\n");
    setColor(15, 0);
}
// Draws a simple standalone header box: top line + title + bottom line
void drawHeader(const char *text) {
    drawHLine(0);
    drawTitleRow(text);
    drawHLine(2);
}
// Draws a sub-header separator with centered label
void drawSubHeader(const char *text) {
    drawHLine(1);
    drawTitleRow(text);
    drawHLine(1);
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

// Giai phong bo nho order
void freeOrder(order **headO){
	order *temp;
	while(*headO != NULL){
		temp = *headO;
		*headO = (*headO)->next;
		free(temp);
	}
}

// Giai phong bo nho shipper
void freeShipper(shipper **headS){
	shipper *temp;
	while(*headS != NULL){
		temp = *headS;
		*headS = (*headS)->next;
		free(temp);
	}
}

// Mac dinh load don hang khi mo chuong trinh
void loadFileOrder(order **head){
	FILE *f = fopen("Order_Information.txt", "r");
	if(f==NULL) return;
	char header[500];
	if(fgets(header, sizeof(header), f)==NULL){
		fclose(f);
		return;
	}
	order *tail = *head;
	if(tail != NULL){
		while(tail->next != NULL)
		tail = tail->next;
	}
	while(!feof(f)){
		char ptr[15];
		char ptr2[20];
		order *newNode = (order*)malloc(sizeof(order));
		if(newNode==NULL) return;
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
			free(newNode);
		}
	}
	fclose(f);
	printf("======DONE LOAD FILE ORDER=====\n");
}

// Ham xoa khoang trang cuoi chuoi
void rtrim(char *str){
    if (str == NULL) return;
    int len = strlen(str);
    while(len > 0 && isspace((unsigned char)str[len - 1])){
        len--;
    }
    str[len] = '\0';
}
//xoa khoang trang
void trimRight(char *str){
	int start=0;
	while (str[start]==' ')
		start++; // xoa khang trang dau
    
    int len = strlen(str); //strlen dung de dem do dai chuoi
    while(len > start && (str[len-1] == ' ' || str[len-1] == '\n' || str[len-1] == '\r')){ //xoa khoang trang + \n + \r cuoi
        len--;// lui lai ktr tiep
    }
    
    if (len > start) {
        memmove(str, str + start, len - start);// don chuoi ve dau
        str[len - start] = '\0'; // ket thuc chuoi
    } else {
        str[0] = '\0'; // chuoi rong
    }
}
void loadFileShipper(shipper **headS) {
    FILE *f = fopen("Shipper_information.txt", "r");
    if (f == NULL) {
        printf("Loi: Khong mo duoc file!\n");
        return;
    }
    // 1. Giai phong danh sach cu truoc khi load moi (Tranh r� r? b? nh?)
    shipper *temp;
    while (*headS != NULL) {
        temp = *headS;
        *headS = (*headS)->next;
        free(temp);
    }
    // Sau khi xoa, *headS chac chan bang NULL
    shipper *tail = NULL;
    char buffer[1000];
    // 2. Doc va bo qua 2 dong tieu de (Header)
    if (fgets(buffer, sizeof(buffer), f) == NULL) {
        fclose(f);
        return;
    }
    if (fgets(buffer, sizeof(buffer), f) == NULL) {
        fclose(f);
        return;
    }
    char line[500];
    while (fgets(line, sizeof(line), f)) {
        // Xoa ky tu xuong dong o cuoi chuoi line neu co
        line[strcspn(line, "\r\n")] = 0;
        shipper *newNode = (shipper*)malloc(sizeof(shipper));
        if (newNode == NULL) break;
        // 3. Sua lai sscanf: Dung %[^|] de doc den khi gap dau gach dung |
        // Luu y: Format nay phu thuoc vao cach ban ghi file. 
        // Duoi day toi gia dinh file dang: Name|CCCD|Code|...
        if (sscanf(line, " %[^|]| %lld | %[^|]| %d | %lf | %d | %d | %d | %d",
                   newNode->Name, &newNode->CCCD, newNode->code, 
                   &newNode->prioritySP, &newNode->weight, 
                   &newNode->x, &newNode->y, 
                   &newNode->numberOrder, &newNode->status) == 9) {
            
            newNode->next = NULL;
            N0S++; // Tang bien dem toan cuc
            // Xoa khoang trang thua o 2 dau chuoi
            trimRight(newNode->Name);
            trimRight(newNode->code);
            // 4. Cap nhat head va link cac node
            if (*headS == NULL) {
                *headS = newNode; // Nut dau tien
                tail = newNode;
            } else {
                tail->next = newNode; // Gan vao duoi
                tail = newNode;
            }
        } else {
            free(newNode); // Neu doc loi thi giai phong luon node nay
        }
    }

    fclose(f);
    printf("====== DONE LOAD FILE SHIPPER =====\n");
}
// Cap nhat don hang khi tat chuong trinh vao file
void SaveOrder(order **headO){
	FILE *f;
	f = fopen("Order_Information.txt", "a");
	if(f==NULL){
		printf("Error!");
		exit(0);
	}
	char *Status[]={"Pending", "Shipping", "Delivered"};
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
			rtrim(temp->orderName);
			rtrim(temp->customerName);
			fprintf(f, "%-5s || %-20s || %-20s || %-3d || %-3d || %-6.2lf || %02d/%02d/%04d || %-10.2lf || %-10s || %-10s\n"
					,temp->code, temp->orderName, temp->customerName, temp->x, temp->y, temp->weight, temp->date.day, 
					temp->date.month, temp->date.year, temp->fee, (temp->priority==1) ? "Express" : "Normal", Status[temp->status]);
			temp->isSaved = 1;
			++count;
		}
		temp = temp->next;
	}
	fclose(f);
	system("start Order_Information.txt");
	if(count!=0)
		printf("\n===There are %d orders in the file===\n", count);
	else
		printf("\n===No orders have been entered into the file\n");
}

// Clear file de sap xep lai
void clear_file(){
	FILE *f = fopen("Order_Information.txt", "w");
	if(f != NULL){
		fclose(f);
	}
}

// Menu chinh - sau moi chuc nang can nhan phim bat ky moi quay lai
void SelectOption(order **headO, shipper **headS) {
    loadFileShipper(headS);
    loadFileOrder(headO);
    hideCursor(); // an con tro chuot

    int pointer = 0; // Vi tr� thanh s�ng (0-4)
    char key;
    int (*Select[4])(order**, shipper**) = {Order_Management, Shipper_Management, Smart_Coordination, Statistics_and_Reports};

    system("cls");
    while(1) {
        goHome();
        Option(pointer);

        key = getch();//doi nhan phim

        if (key == -32) { // Ph�m mui t�n l� ph�m mo rong (phim chuan bi nhap la phim dac biet)
            key = getch();
            if (key == 72) { // Mui t�n l�n
                if (pointer > 0) pointer--;
                else pointer = 4;
            } else if (key == 80) { // Mui t�n xuong
                if (pointer < 4) pointer++;
                else pointer = 0;
            }
        } else if (key == 13) { // Ph�m ENTER
            if (pointer == 4) { // Lua chon tho�t (0)
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
