#include"Lib.h"

// Ham them don hang moi vao danh sach
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

// Ham so sanh 2 ngay, tra ve > 0 neu d1 > d2
int compareDate(DATEE d1, DATEE d2){
    if(d1.year != d2.year) return d1.year - d2.year;
    else if(d1.month != d2.month) return d1.month - d2.month;
    return d1.day - d2.day;
}

// Ham sap xep cac don hang theo ngay tang dan
void SortOrder(order **head){
    if(*head == NULL) return;
    for(order *i = *head; i->next != NULL; i = i->next){
        for(order *j = i->next; j != NULL; j = j->next){
            if(compareDate(i->date, j->date) > 0){
                order *nextI = i->next;
                order *nextJ = j->next;
                order temp = *i;
                *i = *j;
                *j = temp;
                i->next = nextI;
                j->next = nextJ;
            }
        }
    }
    printf("\n======SUCCESS SORT======\n");
}

// Ham tim kiem don hang theo ma don hang
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

// Ham cap nhat thong tin don hang
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

// Ham xu ly logic chinh cua Quan ly don hang
int Order_Management(order **headO, shipper **headS) {
    int pointer = 0;
    char key;

    system("cls");
    while(1) {
        goHome();
        Draw_OrderMenu(pointer);

        key = getch();

        if (key == -32) {
            key = getch();
            if (key == 72) {
                if (pointer > 0) pointer--;
                else pointer = 4;
            } else if (key == 80) {
                if (pointer < 4) pointer++;
                else pointer = 0;
            }
        }
        else if (key == 13) {
            system("cls");
            if (pointer == 4) return 0;

            switch (pointer) {
                case 0: addOrder(headO);    break;
                case 1: SortOrder(headO);   break;
                case 2: FindOrder(headO);   break;
                case 3: UpdateOrder(headO); break;
            }

            printf("\n------------------------------------------\n");
            printf("Action completed. Press any key to return.");
            getch();
            system("cls"); // Xoa man hinh truoc khi ve lai menu
        }
    }
}
