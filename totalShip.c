#include<stdio.h>
#include<stdlib.h>
#include<String.h>
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
double totalShip(order *headO){
    double total = 0;
    if(headO == NULL) return 0.0;
    if(headO->status == 2){
        total = headO->fee;
    }
    return total + totalShip(headO->next);
}
void compileReport(order **headO, shipper **headS, char file[]){
    FILE *f = fopen(file,"w");
    if(f == NULL){
        printf("\nERROR: CANNOT OPEN FILE!");
    }
    fprintf(f,"---------------------------------------------------------------------------------\n");
    fprintf(f,"                      SMART DELIVERY - DAILY SUMMARY REPORT                      \n");
    fprintf(f,"---------------------------------------------------------------------------------\n\n");
    fprintf(f,"============================== PART I: SHIPPER LIST =============================\n");
    shipper *tempS = *headS;
    // xuat code, ten shipper, cccd, trang thai cua shipper
    fprintf(f,"---------------------------------------------------------------------------------\n");
    fprintf(f,"%-7s |%-20s |%-12s |%-10s\n","CODE","NAME SHIPPER","CCCD","STATUS");
    fprintf(f,"---------------------------------------------------------------------------------\n");
    while (tempS != NULL){
        if(tempS->status == 0){
            fprintf(f,"%-7s |%-20s |%-12lld |%-10s\n", tempS->code, tempS->Name, tempS->CCCD, "Available");
        }
        else if(tempS->status == 1){
            fprintf(f,"%-7s |%-20s |%-12lld |%-10s\n", tempS->code, tempS->Name, tempS->CCCD, "Busy");
        }
        tempS=tempS->next;
    }

    fprintf(f,"\n\n");

    fprintf(f,"============================== PART II: ORDER LIST ==============================\n");
    order *tempO = *headO;
    // 0: ton kho, 1: dang giao, 2: da giao
    // xuat code, ten khach hang, can nang, trang thai, tien ship
    fprintf(f,"---------------------------------------------------------------------------------\n");
    fprintf(f,"%-7s |%-20s |%-10s |%-10s |%-10s\n","CODE","CUSTOMER NAME","WEIGHT","STATUS","FEE");
    fprintf(f,"---------------------------------------------------------------------------------\n");
    while (tempO != NULL){
        if(tempO->status == 0){
            fprintf(f,"%-7s |%-20s |%-10.2f |%-10s |%-10.2f\n",tempO->code, tempO->customerName, tempO->weight,"Pending",0.0);
        }
        if(tempO->status ==1){
            fprintf(f,"%-7s |%-20s |%-10.2f |%-10s |%-10.2f\n",tempO->code, tempO->customerName, tempO->weight,"Shipping",0.0);
        }
        if(tempO->status == 2){
            fprintf(f,"%-7s |%-20s |%-10.2f |%-10s |%-10.2f\n",tempO->code, tempO->customerName, tempO->weight,"Delivered",tempO->fee);
        }
        tempO = tempO->next;
    }

    fprintf(f,"\n\n");

    fprintf(f,"============================ PART III: TOTAL REVENUE ============================\n");
    fprintf(f,"---------------------------------------------------------------------------------\n");
    fprintf(f,"TOTAL REVENUE COLLECTED TODAY: %.2f", totalShip(*headO));
    fprintf(f,"---------------------------------------------------------------------------------\n");

    fclose(f);
    printf("DATA HAS BEEN SUCCESSFULLY WRITTEN TO FILE %s!",file);
}
//OPTION 4
int Statistics_and_Reports(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    printf("\n1. Calculate the total daily revenue"
           "\n2. Export the report\n\n");
    do {
        if(countChoice==3) {
            return -1;
        }
        printf("Enter your choice(1-2): ");
        scanf("%d", &choiceTwo);
        ++countChoice;
    } while(choiceTwo>2 || choiceTwo<1);
    switch (choiceTwo){
        case 1:
        printf("TOTAL REVENUE DAILY: %.2lf",totalShip(*headO));
        break;
        case 2:
        compileReport(headO,headS,"Compile_Report.txt");
        break;
    }
}
//giai phong bo nho order
void freeOrder(order **headO) {
    order *temp;
    while(*headO != NULL) {
        temp = *headO;
        *headO = (*headO)->next;
        free(temp);
    }
}
//giai phong bo nho shipper
void freeShipper(shipper **headS) {
    shipper *temp;
    while(*headS != NULL) {
        temp = *headS;
        *headS = (*headS)->next;
        free(temp);
    }
}
int main() {
    // 1. Khoi tao danh sach Shipper mau
    shipper *s1 = (shipper*)malloc(sizeof(shipper));
    strcpy(s1->code, "S001");
    strcpy(s1->Name, "Nguyen Van Hung");
    s1->CCCD = 123456789012;
    s1->status = 0; // Available
    
    shipper *s2 = (shipper*)malloc(sizeof(shipper));
    strcpy(s2->code, "S002");
    strcpy(s2->Name, "Le Thi Tuoi");
    s2->CCCD = 987654321098;
    s2->status = 1; // Busy
    
    s1->next = s2;
    s2->next = NULL;

    // 2. Khoi tao danh sach Don hang mau
    order *o1 = (order*)malloc(sizeof(order));
    strcpy(o1->code, "ORD1");
    strcpy(o1->customerName, "Anh Tuan");
    o1->weight = 2.5;
    o1->fee = 55000;
    o1->status = 2; // Delivered (Se duoc tinh vao doanh thu)
    
    order *o2 = (order*)malloc(sizeof(order));
    strcpy(o2->code, "ORD2");
    strcpy(o2->customerName, "Chi Lan");
    o2->weight = 1.0;
    o2->fee = 20000;
    o2->status = 0; // Pending
    
    o1->next = o2;
    o2->next = NULL;

    // 3. Goi ham menu de nguoi dung thao tac
    Statistics_and_Reports(&o1, &s1);

    // 4. Giai phong bo nho truoc khi thoat
    free(o1); free(o2);
    free(s1); free(s2);

    return 0;
}





