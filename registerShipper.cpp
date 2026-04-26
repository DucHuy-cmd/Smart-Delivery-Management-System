//OPTION 2
int Shipper_Management(order**headO,shipper **headS);
void registerShipper (shipper **headS);
void deleteShipper (shipper **headS);
void informationShipper (shipper **headS);
void loadFileShipper(shipper **headS);

static int N0S=1;
//Mỗi 1 shipper là 1 Node chưa dc liên kết
//Mỗi 1 Node này cần dc điền Thông tin
shipper *CreateShipper () {
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

    //Phan loai shipper 0: Hỏa tốc, 1: bình thường
    printf ("Enter Your Shipper Type: ");
    scanf ("%d",&newShipper->prioritySP);
    while (getchar() != '\n');
    
    while (newShipper->prioritySP!=0 && newShipper->prioritySP!=1) {
        printf ("Invalid Type");
        scanf ("%d",&newShipper->prioritySP);
        while (getchar() != '\n');
    }

    //Nhận Mã shipper
    sprintf (newShipper->code,"SP%02d",N0S++);
    newShipper->numberOrder=0;
    newShipper->status=0;
    newShipper->x=0;
    newShipper->y=0;
    printf ("\n");

    newShipper->next=NULL;
    return newShipper;
}

void informationShipper (shipper **headS) {
    shipper *p=*headS;
    FILE *S = fopen("Shipper_information.txt","w");
    if (S==NULL) {
        printf ("ERROR!");
        return;
    }
    fprintf (S,"%-30s||%-20s||%-10s||%-10s||%-6s||%-6s||%-6s||%-16s||%-6s||\n","Name","CCCD","CODE","Priority","Weight","x","y","Number of Orders","Status");
    while (p!=NULL) {
        fprintf (S,"%-30s||%-20lld||%-10s||%-10d||%-6.2lf||%-6d||%-6d||%-16d||%-6d||\n",p->Name,p->CCCD,p->code,p->prioritySP,p->weight,p->x,p->y,p->numberOrder,p->status);
        p=p->next;
    }
    fclose(S);
}

//xoa khoang trang
void trimRight(char *str){
	int start=0;
	while (str[start]==' ')
		start++; // xoa khang trang dau
    
    int len = strlen(str); //strlen dung de dem do dai chuoi
    while(len > 0 && (str[len-1] == ' ' || str[len-1] == '\n' || str[len-1] == '\r')){ //xóa khoảng trắng + \n + \r cuối
        len--;// lui lai ktr tiep
    }
    
    memmove(str, str + start, len - start);// don chuoi ve dau
    str[len - start] = '\0'; // ket thuc chuoi
}

void loadFileShipper(shipper **headS){ // ghi vao main tren selectoption
	FILE *f = fopen("Shipper_information.txt", "r");
	if(f==NULL) 
        return;
    shipper *temp;// giai phong list cu ( tranh tran bo nho ) 
    while (*headS!=NULL){
        temp=*headS;
        *headS=(*headS)->next;
        free(temp);
    }
    shipper *tail=NULL;
    char buffer[1000];
    fgets (buffer ,sizeof(buffer), f);//doc va bo qua dong dau
    
    char line[500];//doc tung dong -> sscanf de tach du lieu vao struct 
    while (fgets(line,sizeof(line),f)){
        shipper *newNode = (shipper*)malloc(sizeof(shipper));
        if(newNode == NULL){
            fclose(f);
            return;
        }
        if (sscanf(line," %[^|] || %lld || %[^|] || %d || %lf || %d || %d || %d || %d ||",//doc du lieu r gan vao struct
            newNode->Name, &newNode->CCCD, newNode->code, &newNode->prioritySP, &newNode->weight, &newNode->x, &newNode->y
            ,&newNode->numberOrder, &newNode->status)==9){
                N0S++;// phai tang neu ko se co thag trung ma shipper
                newNode->next=NULL;
            }
        else{
            free(newNode);
            continue;// bo qua vong lap nhay sang tiep theo khi sscanf !=9
        }    
        trimRight(newNode->Name); // dung cho ten
        trimRight(newNode->code);// ma shipper
        if (*headS==NULL){
            *headS=newNode;
            tail=newNode;
        }
        else {
            tail->next=newNode;
            tail=newNode;
        }
    }
	fclose(f);
	printf("======DONE LOAD FILE SHIPPER=====\n");
}
void registerShipper (shipper **headS) {
    shipper *Tail = *headS;
    while (Tail && Tail->next != NULL) {
        Tail = Tail->next;
    }
    //nhập số lượng shipper chuẩn bị cung cấp thông  tin (có thể nhiều hoặc ít)
    //để linh hoạt trong việc cung cấp thông tin
    //NoS = Number of Shhipper
    printf ("Enter Number of Shipper: ");
    int NoS;
    scanf ("%d",&NoS);
    while (getchar() != '\n');

    for (int i=0; i<NoS; i++) {
        //Nhập thông tin của n shipper đã đăng ký
        shipper *newShipPtr=CreateShipper();
        if (*headS==NULL) {
            *headS=newShipPtr;
            Tail=newShipPtr;
        }
        else {
            Tail->next=newShipPtr;
            Tail = newShipPtr;
        }
    }
    informationShipper (headS);
    printf ("\n====SUCCESS REGISTED====\n");
}

void deleteShipper (shipper **headS) {
    if (*headS ==NULL) {
        printf ("List is empty");
        return;
    }
    shipper *p=*headS;
    char deLShip[5];
    printf ("Enter The Shipper ID You Want To Delete: ");
    while (getchar() != '\n');
    fgets (deLShip,sizeof(deLShip),stdin);
    deLShip[strcspn(deLShip,"\n")] = '\0';
    if (strcmp(p->code, deLShip)==0) {
        *headS=(*headS)->next;
        free(p);
        printf ("Delete Successfully!\n");
        informationShipper (headS);
        return;
    }
    while (p->next!=NULL) {
        shipper *p1=p->next;
        if (p->next!=NULL && strcmp(p->next->code,deLShip)==0) {
            p->next=p1->next;
            free(p1);
            printf ("Delete Successfully!\n");
            informationShipper (headS);
            return;
        }
        p=p->next;
    }
    printf ("Shipper Not Found!!\n");
    informationShipper (headS);
}

int Shipper_Management(order** headO,shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    printf("\n1. Register as a new Shipper"
           "\n2. Delete Shipper"
           "\n3. List Shipper\n\n");
    do {
        if(countChoice==3) {
            return -1;
        }
        printf("Enter your choice(1-3): ");
        scanf("%d", &choiceTwo);
        ++countChoice;
    } while(choiceTwo>3 || choiceTwo<1);
    switch(choiceTwo) {
    case 1:
        registerShipper(headS);
        break;
    case 2:
        deleteShipper(headS);
        break;
    case 3:
    	system("notepad Shipper_information.txt");
    	break;
    }
}
