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
            fprintf(f,"%-7s |%-20s |%-10.2f |%-10s |%-10.2lf\n",tempO->code, tempO->customerName, tempO->weight,"Delivered",tempO->fee);
        }
        tempO = tempO->next;
    }

    fprintf(f,"\n\n");

    fprintf(f,"============================ PART III: TOTAL REVENUE ============================\n");
    fprintf(f,"---------------------------------------------------------------------------------\n");
    fprintf(f,"                       TOTAL REVENUE COLLECTED TODAY:                         %.2f\n", totalShip(*headO));
    fprintf(f,"---------------------------------------------------------------------------------\n");

    fclose(f);
    printf("DATA HAS BEEN SUCCESSFULLY WRITTEN TO FILE %s!",file);
}
//OPTION 4
int Statistics_and_Reports(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    do{
    printf("\n1. Calculate the total daily revenue"
           "\n2. Export the report"
           "\n3.Back to menu\n\n");
    do {
        if(countChoice==3) {
            return -1;
        }
        printf("Enter your choice(1-3): ");
        scanf("%d", &choiceTwo);
        ++countChoice;
    } while(choiceTwo>3 || choiceTwo<1);
    switch (choiceTwo){
        case 1:
           printf("TOTAL REVENUE DAILY: %.2lf",totalShip(*headO));
           break;
        case 2:
           compileReport(headO,headS,"Compile_Report.txt");
           break;
        case 3:
           return 0;
        default:
           countChoice++;
           printf("\nInvalid choice! (Attempts: %d/3)\n", countChoice);
           break;
}
    printf("\nPress Enter To Return");
    getch();
    if(choiceTwo>=1 && choiceTwo <3) countChoice = 0;
    if(countChoice ==3) return -1;
}while(choiceTwo !=3);
}
