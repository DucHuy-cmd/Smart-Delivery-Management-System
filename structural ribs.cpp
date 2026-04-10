#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
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
//Nhap vao so hang moi
void addOrder(order **headO){
	int number;
	printf("How many order: ");
	scanf("%d", &number);
	for(int i = 0; i<number;i++){
		printf("\n---Input order %d/%d---\n", i+1, number);
		order *newNode = (order*)malloc(sizeof(order));
		if(newNode==NULL) return;
		printf("Enter code: ");
		scanf("%s", newNode->code);
		getchar();
		printf("Enter name order: ");
		fgets(newNode->orderName, sizeof(newNode->orderName), stdin);
		newNode->orderName[strcspn(newNode->orderName, "\n")]=0;
		printf("Enter customer name: ");
		fgets(newNode->customerName, sizeof(newNode->customerName), stdin);
		newNode->customerName[strcspn(newNode->customerName, "\n")]=0;
		printf("Enter delivery coordinates: ");
		scanf("%d%d", &newNode->x, &newNode->y);
		printf("Enter cargo weight: ");
		scanf("%lf", &newNode->weight);
		newNode->isSaved = 0;
		//0: giao bth, 1: giao hoa toc
		do{
			printf("Enter priority(0 || 1): ");
			scanf("%d", &newNode->priority);
		}while(newNode->priority != 0 && newNode->priority != 1);
		printf("Enter shipping fee: ");
		scanf("%lf", &newNode->fee);
		//0: ton kho, 1: dang giao, 2: da giao
		do{
			printf("Enter status order: ");
			scanf("%d", &newNode->status);
		}while(newNode->status!=0 && newNode->status!=1 && newNode->status!=2);
		printf("Enter time order: ");
		scanf("%d%d%d", &newNode->date.day, &newNode->date.month, &newNode->date.year);
		newNode->next = NULL;
		order *tail = *headO;
//truong hop da co don, khac NULL thi phai tim toi khi nao moc xich chua gan vao  
		if(tail != NULL){
			while(tail->next!=NULL){
				tail = tail->next;
			}
		}
		if(*headO==NULL){
			*headO = newNode;
			tail = newNode;
		}
		else{
			tail->next = newNode;
			tail = newNode;
		}
	}
	printf("\n====SUCCESS ADD ORDER====\n");
}
//Cap nhat don hang vao file
void UpdateOrder(order **headO){
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
		fprintf(f, "%-5s || %-10s || %-20s || %-3s || %-3s || %-6s || %-10s || %-10s || %-10s || %-10s\n",
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
			fprintf(f, "%-5s || %-10s || %-20s || %-3d || %-3d || %-6.2lf || %02d/%02d/%04d || %-10.2lf || %-10s || %-10s\n"
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
//Hien thi danh sach cho(ton kho) cua don hang
void DisplayOrderPending(order **headO){
	order *temp = *headO;
	if(temp!=NULL)
		printf("%-20s %-20s\n", "Customer Name", "Order Name");
	else{
		printf("\nNo Display Order Pending\n");
	}
	while(temp != NULL){
		if(temp->status==0){
			printf("%-20s %-20s\n", temp->customerName, temp->orderName);
		}
		temp = temp->next;
	}
}
//OPTION 1
int Order_Management(order **headO, shipper **headS){
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Add a new order"
			"\n2. Display the list of pending orders"
			"\n3. Update the order to the file\n\n");
	do{
		if(countChoice==3){
			return -1;
		}		
		printf("Enter your choice(1-3): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>3 || choiceTwo<1);
	switch(choiceTwo){
		case 1: 
			addOrder(headO);
			break;
		case 2:
			DisplayOrderPending(headO);
			break;	
		case 3:
			UpdateOrder(headO);
			break;
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
//OPTION 5
int System_Menu(order **headO, shipper **headS){
	int choiceTwo;
	int countChoice = 0;
	printf("\n1. Save the data to a file"
			"\n2. Load data from a file at startup\n\n");
	do{
		if(countChoice==3){
			return -1;
		}
		printf("Enter your choice(1-2): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>2 || choiceTwo<1);
}
void Option(){
	printf("================================================\n");
	printf("= %-44s =\n", "    SMART DELIVERY AND MANAGEMENT SYSTEM");
	printf("================================================\n");
	printf("= %-44s =\n", "[1]. ORDER MANAGEMENT");
	printf("= %-44s =\n", "[2]. SHIPPER MANAGEMENT");
	printf("= %-44s =\n", "[3]. SMART COORDINATION");
	printf("= %-44s =\n", "[4]. STATISTICS AND REPORTS");
	printf("= %-44s =\n", "[5]. SYSTEM MENU");
	printf("= %-44s =\n", "[0]. EXIT PROGRAM");
	printf("================================================\n");
}
//thuc hien chon de thuc hien va dieu kien neu nhap dung thi thuc hien, sai thi cho nhap lai
//sai qua 3 lan thi khoa tai khoan		
void SelectOption(order **headO, shipper **headS){
	int choice, choiceTwo;
	int countChoice = 0;
	int (*Select[5])(order**, shipper**) = {Order_Management, Shipper_Management, Smart_Coordination, Statistics_and_Reports, System_Menu};
	while(1){
		Option();
		printf("Enter your choice(1-5): ");
		scanf("%d", &choice);
		if(choice==0){
			printf("\nExit Program...GoodBye!");
			return;
		}
		else if(choice>=1 && choice<=5){
			int check = Select[choice-1](headO, headS);
			countChoice = 0;
			if(check==-1){
				printf("\n===SYSTEM LOCKED!===");
				return;
			}
		}
		else{
			++countChoice;
			if(countChoice>=3){
				printf("\n===SYSTEM LOCKED!===");
				return;
			}
			printf("\nYou enter the wrong %d information\n\n", countChoice);
		}
		printf("Press any key to return to Menu.\n");
		getch();//an phim bat ky de quay lai menu
		system("cls");//clear man hinh
	}
}
int main(){
	order *headOrder = NULL;
	shipper *headShipper = NULL;
	SelectOption(&headOrder, &headShipper);
}
