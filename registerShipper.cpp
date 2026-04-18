int Shipper_Management(order **headO, shipper **headS);
void registerShipper (order **headO, shipper **headS);
static int N0S=1;
//Mỗi 1 shipper là 1 Node chưa dc liên kết
//Mỗi 1 Node này cần dc điền Thông tin
shipper *CreateShipper (){
    shipper *newShipper=(shipper*)malloc(sizeof(shipper));
    if(newShipper == NULL)
        return NULL;
    
    printf ("Enter Your Name: ");
    fgets (newShipper->Name,sizeof(newShipper->Name),stdin);
    newShipper->Name[strcspn(newShipper->Name, "\n")]='\0';
    	
    printf ("Enter Your CCCD: ");
    scanf("%lld", &newShipper->CCCD);
    while (getchar() != '\n');
    
    //trọng lượng tối đa cảu shipper 
    printf ("Enter Your Order Weight: ");
    scanf ("%lf",&newShipper->weight);
    while (getchar() != '\n');
    
    //Nhập lượng đơn hàng đã nhận;
    printf ("Enter Your Order Number: ");
    scanf ("%d",&newShipper->numberOrder);
    while (getchar() != '\n');
    
    //Phan loai shipper 0: Hỏa tốc, 1: bình thường 
    printf ("Enter Your Shipper Type: ");
    scanf ("%d",&newShipper->prioritySP);
	while (getchar() != '\n');
    while (newShipper->prioritySP!=0 && newShipper->prioritySP!=1){
        printf ("Invalid Type");
        scanf ("%d",&newShipper->prioritySP);
		while (getchar() != '\n');
    }
    
    //Nhận Mã shipper
    sprintf (newShipper->code,"SP%02d",N0S++);
    
    newShipper->status=0;
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

void registerShipper (order **headO, shipper **headS){
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
    
    for (int i=0;i<NoS;i++){
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
    printf ("Press Enter To Return");
    char c;
    c = getch();
    while (c!='\r'){
        c=getch();
    }
    Shipper_Management(headO,headS);
}

void deleteShipper (order **headO,shipper **headS){
    if (*headS ==NULL){
        printf ("List is empty");
        return;
    }
    int F=0;
    shipper *p=*headS;
    char deLShip[5];
    printf ("Enter The Shipper ID You Want To Delete: ");
    fgets (deLShip,sizeof(deLShip),stdin);
    deLShip[strcspn(deLShip,"\n")] = '\0';
    if (strcmp(p->code, deLShip)==0){
        *headS=(*headS)->next;
        free(p);
        printf ("Delete Successfully!\n");
        F=1;
    }
    while (p->next!=NULL){
        shipper *p1=p->next;
        if (p->next!=NULL && strcmp(p->next->code,deLShip)==0){
            p->next=p1->next;
            p1->next=NULL;
            free(p1);
            printf ("Delete Successfully!\n");
            F=1;
            break;
        }
        p=p->next;
    }
    if (F==0){
        printf ("Shipper Not Found!!\n");
    }
    printf ("Press Enter To Return");
    char c;
    c = getch();
    while (c!='\r'){
        c=getch();
    }
    informationShipper (headS);
    Shipper_Management(headO,headS);
}

int Shipper_Management(order **headO, shipper **headS){
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Register as a new Shipper"
			"\n2. Delete Shipper"
			"\n3. Return\n\n");
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
			registerShipper(headO,headS);
			break;
		case 2:
		    deleteShipper(headO,headS);
			break;	
		case 3:
			SelectOption(headO,headS);
			break;
	}
}
