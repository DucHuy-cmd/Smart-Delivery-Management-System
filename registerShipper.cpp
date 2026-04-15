//them thu vien
#include<window.h>

//OPTION 2
void registerShipper (shipper **headS);
void numberOfOrders (order **headO,shipper **headS);
void statusShipper (shipper **headS);
static int N0S=1;
void EnterInformation (shipper *A){
    printf ("Enter Your Name: ");
    fgets (A->Name,sizeof(A->Name),stdin);
    A->Name[strcspn(A->Name, "\n")]='\0';
    	
    printf ("Enter Your CCCD: ");
    scanf("%lld", &A->CCCD);
    while (getchar() != '\n');
    
   	//trọng lượng tối đa cảu shipper 
    printf ("Enter Your Order Weight: ");
    scanf ("%lf",&A->weight);
    while (getchar() != '\n');
    
    //Nhập lượng đơn hàng đã nhận;
    printf ("Enter Your Order Number: ");
    scanf ("%d",&A->numberOrder);
    while (getchar() != '\n');
    
    //Nhận Mã shipper
    sprintf (A->code,"SP%02d",N0S++);
}

//Mỗi 1 shipper là 1 Node chưa dc liên kết
shipper *CreateShipper (){
    shipper *newShipper=(shipper*)malloc(sizeof(shipper));
    if(newShipper == NULL)
        return NULL;
        
    EnterInformation(newShipper);
    newShipper->x=0;
    newShipper->y=0;
    printf ("\n");
    
    newShipper->next=NULL;
    return newShipper;
}

void informationShipper (shipper **headS){
	shipper *p=*headS;
	FILE *S = fopen("information.txt","w");
	if (S==NULL){
		printf ("ERROR!");
		return;
	}
	fprintf (S,"====================================List shipper==================================\n");
	fprintf (S,"----------------------------------------------------------------------------------\n");
	fprintf (S,"||%-30s||%-20s||%-10s||%-10s||\n","Name","CCCD","CODE","Order");
	while (p!=NULL){
		fprintf (S,"||%-30s||%-20lld||%-10s||%-10d\n",p->Name,p->CCCD,p->code,p->numberOrder);
		p=p->next;
	}
	fclose(S);
}

void registerShipper (shipper **headS){
    shipper *Tail = *headS;
	while (Tail && Tail->next != NULL){
    	Tail = Tail->next;
	}
    //nhập số lượng shipper chuẩn bị cung cấp thông  tin (có thể nhiều hoặc ít)
    //để linh hoạt trong việc cung cấp thông tin
    //NoS = Number of Shhipper
    printf ("Enter Number of Shipper: ");
    int NoS;
    scanf ("%d",&NoS);
    while (getchar() != '\n');
    
    for (int i=0;i< NoS;i++){
    //Nhập thông tin của n shipper đã đăng ký
    shipper *newShipPtr=CreateShipper();
    	if (*headS==NULL){
    		*headS=newShipPtr;
    		Tail=newShipPtr;
		}
		else{
			Tail->next=newShipPtr;
			Tail = newShipPtr;
		}
		Tail->status=0;
    }
	informationShipper (headS);
    printf ("\n====SUCCESS REGISTED====\n");
}

void numberOfOrders (order **headO,shipper **headS){
	shipper *S=*headS;
	if (*headS == NULL || *headO == NULL) return;
	double W=S->weight;
	while (S != NULL){
		W=S->weight;// gan lai Max Weight cua Shipper
		S->numberOrder=0;
		order *O=*headO;// Do lại tu dau den cuoi
		while (O!=NULL){
			if (O->weight <= W && O->status==0){
				W=W-O->weight;
				S->numberOrder++;
				O->status=1;
			}
			O=O->next;
		}
		S->status = (S->numberOrder == 0) ? 0 : 1;
		S=S->next;
	}
}

void missionShipper (order **headO,shipper **headS){
	shipper *S=*headS;
	while (S!=NULL){
		order *O=*headO;// dat lai con tro ve dau
		while (O!=NULL){
			if (S->x==O->x && S->y==O->y && O->status == 1){
				S->numberOrder--;
				O->status=2;
			}
			O=O->next;
		}
	S->status = (S->numberOrder == 0) ? 0 : 1;
	S=S->next;	
	}
}	

void simulate (order **headO,shipper **headS){
	int time = 0;
	while (1){
		numberOfOrders(headO,headS);
		missionShipper(headO,headS);
		
		informationShipper(headS);
		
		time+=8;
		Sleep(1000);
	}
}

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
	switch(choiceTwo){
		case 1: 
			registerShipper(headS);
			break;
		/*case 2:
			DisplayOrderPending(headO);
			break;	
		case 3:
			UpdateOrder(headO);
			break;*/
	}
}
