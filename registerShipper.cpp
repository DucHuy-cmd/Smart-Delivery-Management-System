void registerShipper (shipper **headS);
void numberOfOrders (order **headO,shipper **headS);
void statusShipper (shipper **headS);

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
    while (newShipper->prioritySP!=0 || newShipper->prioritySP!=1){
        printf ("Invalid Type");
        scanf ("%d",&newShipper->prioritySP);
    }
    
    //Nhận Mã shipper
    sprintf (newShipper->code,"SP%02d",N0S++);
    
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
}

void deleteShipper (shipper **headS){
    if (*headS ==NULL){
        printf ("List is empty");
        return;
    }
    shipper *p=*headS;
    char deLShip[5];
    printf ("Enter The Shipper ID You Want To Delete: ");
    fgets (deLShip,sizeof(deLShip),stdin);
    deLShip[strcspn(deLShip,"\n")] = '\0';
    if (strcmp(p->code, deLShip)==0){
        *headS=(*headS)->next;
        free(p);
        printf ("Delete Successfully!\n");
        return;
    }
    if (p->next!=NULL){
    shipper *p1=p->next;
        while (p->next!=NULL){
            if (p->next!=NULL && strcmp(p->next->code,deLShip)==0){
                p->next=p1->next;
                p1->next=NULL;
                free(p1);
                printf ("Delete Successfully!\n");
                return;
            }
            else{
                p=p->next;
                p1=p1->next;
            }
        }
    }
    printf ("Shipper Not Found!!\n");
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
		case 2:
		    deleteShipper(headS);
			break;	
		/*case 3:
			UpdateOrder(headO);
			break;*/
	}
}        
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
			if (O->weight <= W && O->status==0 && S->status==0){
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

//mô phỏng he thong cap nhat don hang moi 8h
void simulate (order **headO,shipper **headS){
	while (1){
		numberOfOrders(headO,headS);
		missionShipper(headO,headS);
		
		informationShipper(headS);
		//mô phong 1 phut = 8 tieng;
		Sleep(60000);
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
