//OPTION 2
// Ham tao moi mot Shipper
shipper *CreateShipper() {
    shipper *newShipper = (shipper*)malloc(sizeof(shipper));
    if(newShipper == NULL) return NULL;

    printf("  \xAF Full Name: ");
    fgets(newShipper->Name, sizeof(newShipper->Name), stdin);
    newShipper->Name[strcspn(newShipper->Name, "\n")] = '\0';

    printf("  \xAF Citizen ID: ");
    scanf("%lld", &newShipper->CCCD);
    while(getchar() != '\n');

    printf("  \xAF Max Transport Weight (kg): ");
    scanf("%lf", &newShipper->weight);
    while(getchar() != '\n');

    int type;
    do {
        printf("  \xAF Shipper Type (0: Express, 1: Normal): ");
        scanf("%d", &type);
        while(getchar() != '\n');
    } while(type != 0 && type != 1);
    newShipper->prioritySP = type;

    sprintf(newShipper->code, "SP%02d", N0S++);
    newShipper->numberOrder = 0;
    newShipper->status = 0;
    newShipper->x = 0;
    newShipper->y = 0;
    newShipper->next = NULL;
    return newShipper;
}

// Ham ghi thong tin Shipper vao file
// Tong chieu rong khung = 108 ky tu (inner = 106)
// Border : +=========...=========+  (108 chars)
// Sep    : +---------...---------+  (108 chars)
// Row    : | col... |             (108 chars)
void informationShipper(shipper **headS) {
    shipper *p = *headS;

    FILE *data    = fopen("Shipper_data.txt", "w");
    FILE *display = fopen("Shipper_information.txt", "w");

    #define ITOP "+==========================================================================================================+\n"
    #define ISEP "+----------------------------------------------------------------------------------------------------------+\n"

    /* Header box */
    fprintf(display, ITOP);
    fprintf(display, "|%41s%-24s%41s|\n", "", "SHIPPER INFORMATION LIST", "");
    fprintf(display, ITOP);

    /* Column headers */
    fprintf(display, "| %-24s | %-13s | %-7s | %-7s | %-7s | %-4s | %-4s | %-7s | %-7s |\n",
            "NAME", "CCCD", "CODE", "TYPE", "WEIGHT", "X", "Y", "ORDERS", "STATUS");
    fprintf(display, ISEP);

    while(p != NULL) {
        /* Data file (raw, dung de doc lai bang sscanf) */
        fprintf(data, "%s|%lld|%s|%d|%lf|%d|%d|%d|%d\n",
                p->Name, p->CCCD, p->code, p->prioritySP,
                p->weight, p->x, p->y, p->numberOrder, p->status);

        /* Display file */
        fprintf(display, "| %-24s | %-13lld | %-7s | %-7d | %-7.2lf | %-4d | %-4d | %-7d | %-7d |\n",
                p->Name, p->CCCD, p->code, p->prioritySP,
                p->weight, p->x, p->y, p->numberOrder, p->status);
        p = p->next;
    }

    fprintf(display, ITOP);

    #undef ITOP
    #undef ISEP

    fclose(data);
    fclose(display);
}
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
void loadFileShipper(shipper **headS){
	FILE *f = fopen("Shipper_data.txt", "r");
	if(f==NULL) {
        return;
	}
    shipper *temp;// giai phong list cu ( tranh tran bo nho ) 
    while (*headS!=NULL){
        temp=*headS;
        *headS=(*headS)->next;
        free(temp);
    }
    shipper *tail=NULL;
    int max=0;
    char line[500];//doc tung dong -> sscanf de tach du lieu vao struct 
    while (fgets(line,sizeof(line),f)){
        shipper *newNode = (shipper*)malloc(sizeof(shipper));
        if(newNode == NULL){
            fclose(f);
            return;
        }
        int check = sscanf(line, "%[^|]|%lld|%[^|]|%d|%lf|%d|%d|%d|%d",
    newNode->Name, &newNode->CCCD, newNode->code, &newNode->prioritySP,
    &newNode->weight, &newNode->x, &newNode->y,
    &newNode->numberOrder, &newNode->status);
        newNode->next=NULL;
   	         
        if (check != 9){
            free(newNode);
            continue;// bo qua vong lap nhay sang tiep theo khi sscanf !=9
        }    
        trimRight(newNode->Name);
        trimRight(newNode->code);

		max++;// phai tang neu ko se co thag trung ma shipper
        if (*headS==NULL){
            *headS=newNode;
            tail=newNode;
        }
        else {
            tail->next=newNode;
            tail=newNode;
        }
    }
    N0S=max+1;
	fclose(f);
	printf("======DONE LOAD FILE SHIPPER=====\n");
}
// Ham dang ky them cac Shipper moi
void registerShipper(shipper **headS) {
    int NoS;
    drawHeader("REGISTER NEW SHIPPER");
    printf("\n  [?] How many shippers to register: ");
    scanf("%d", &NoS);
    while(getchar() != '\n');

    shipper *Tail = *headS;
    while(Tail && Tail->next != NULL) Tail = Tail->next;

    for(int i = 0; i < NoS; i++) {
        char title[64];
        sprintf(title, "SHIPPER REGISTRATION %d / %d", i+1, NoS);
        drawHeader(title);
        shipper *newShipPtr = CreateShipper();
        if(*headS == NULL) {
            *headS = newShipPtr;
            Tail = newShipPtr;
        } else {
            Tail->next = newShipPtr;
            Tail = newShipPtr;
        }
    }
    informationShipper(headS);
    printf("\n  ------------------------------------------\n");
    printf("  [!] SUCCESS: %d SHIPPERS REGISTERED.\n", NoS);
}

// Ham xoa mot Shipper theo ma ID
void deleteShipper(shipper **headS) {
    if(*headS == NULL) {
        printf("List is empty");
        return;
    }
    shipper *p = *headS;
    char deLShip[5];
    printf("Enter The Shipper ID You Want To Delete: ");
    while(getchar() != '\n');
    fgets(deLShip, sizeof(deLShip), stdin);
    deLShip[strcspn(deLShip, "\n")] = '\0';

    if(strcmp(p->code, deLShip) == 0) {
        *headS = (*headS)->next;
        free(p);
        printf("Delete Successfully!\n");
        informationShipper(headS);
        return;
    }
    while(p->next != NULL) {
        shipper *p1 = p->next;
        if(p->next != NULL && strcmp(p->next->code, deLShip) == 0) {
            p->next = p1->next;
            free(p1);
            printf("Delete Successfully!\n");
            informationShipper(headS);
            return;
        }
        p = p->next;
    }
    printf("Shipper Not Found!!\n");
    informationShipper(headS);
}

// Ham ve giao dien Menu Quan ly Shipper
void Draw_ShipperMenu(int pointer) {
    char *options[] = {
        "Register a new Shipper",
        "Delete a Shipper",
        "View Shipper list (Notepad)",
        "BACK TO MAIN MENU"
    };
    char buf[MENU_W + 2];
    drawHLine(0);
    drawTitleRow("[ 2 ]   SHIPPER MANAGEMENT");
    drawHLine(1);
    drawEmptyRow();
    for(int i = 0; i < 4; i++) {
        if(i == pointer)
            sprintf(buf, "  > [ %d ]. %s", (i == 3 ? 0 : i + 1), options[i]);
        else
            sprintf(buf, "    [ %d ]. %s", (i == 3 ? 0 : i + 1), options[i]);
        drawItemRow(buf, i == pointer);
    }
    drawEmptyRow();
    drawHLine(1);
    drawHintRow("    [^][v] Navigate   |   [ENTER] Select");
    drawHLine(2);
}

// Ham xu ly logic chinh cua Quan ly Shipper
int Shipper_Management(order **headO, shipper **headS) {
    int pointer = 0;
    char key;

    system("cls");
    while(1) {
        goHome();
        Draw_ShipperMenu(pointer);

        key = getch();

        if(key == -32) {
            key = getch();
            if(key == 72) {
                if(pointer > 0) pointer--;
                else pointer = 3;
            } else if(key == 80) {
                if(pointer < 3) pointer++;
                else pointer = 0;
            }
        }
        else if(key == 13) {
            system("cls");
            if(pointer == 3) return 0;

            switch(pointer) {
                case 0: registerShipper(headS); break;
                case 1: deleteShipper(headS);   break;
                case 2:
                    printf("Opening Shipper_information.txt in Notepad...\n");
                    system("notepad Shipper_information.txt");
                    break;
            }
            if(pointer != 2) {
                printf("\n------------------------------------------\n");
                printf("Action completed. Press any key to return.");
                getch();
                system("cls"); // Xoa man hinh truoc khi ve lai menu
            } else {
                system("cls"); // Xoa man hinh cho truong hop mo notepad
            }
        }
    }
}
