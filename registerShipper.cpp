void registerShipper (shipper **headS);
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
	fprintf (S,"||%-30s||%-20s||%-10s||\n","Name","CCCD","CODE");
	while (p!=NULL){
		fprintf (S,"||%-30s||%-20lld||%-10s||\n",p->Name,p->CCCD,p->code);
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
    }
   	informationShipper (headS);
    printf ("\n====SUCCESS REGISTED====\n");
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
