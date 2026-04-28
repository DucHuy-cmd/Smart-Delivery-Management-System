#include "Lib.h"
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
    fprintf(f,"                       TOTAL REVENUE EARNED:                                  %.2f\n", totalShip(*headO));
    fprintf(f,"---------------------------------------------------------------------------------\n");

    fclose(f);
    printf("DATA HAS BEEN SUCCESSFULLY WRITTEN TO FILE %s!",file);
}
void Draw_StatisticsMenu(int pointer) {
    char *options[] = {
        "Calculate the total daily revenue",
        "Export the report (Compile_Report.txt)",
        "BACK TO MAIN MENU"
    };

    printf("  ================================================\n");
    printf("  |           4. STATISTICS AND REPORTS          |\n");
    printf("  ================================================\n");

    for (int i = 0; i < 3; i++) {
        if (i == pointer) {
            printf("  | ");
            setColor(0, 15); // Highlight
            printf("> [%d]. %-35s", (i == 2 ? 0 : i + 1), options[i]);
            setColor(15, 0);
            printf(" |\n");
        } else {
            printf("  |    [%d]. %-37s |\n", (i == 2 ? 0 : i + 1), options[i]);
        }
    }
    printf("  ================================================\n");
}
//OPTION 4
int Statistics_and_Reports(order **headO, shipper **headS) {
    int pointer = 0;
    char key;

    while (1) {
        system("cls");
        printf("\n");
        Draw_StatisticsMenu(pointer);
        printf("\n  (Dung mui ten de di chuyen, ENTER de chon)");

        key = getch();

        if (key == -32) { // Bat phim mui ten
            key = getch();
            if (key == 72) { // Len
                if (pointer > 0) pointer--;
                else pointer = 2; 
            } else if (key == 80) { // Xuong
                if (pointer < 2) pointer++;
                else pointer = 0; 
            }
        } 
        else if (key == 13) { // Phim ENTER
            system("cls");
            if (pointer == 2) return 0; // Quay lai Menu chinh

            switch (pointer) {
                case 0:
                    printf("\n>>> REVENUE CALCULATION <<<\n");
                    printf("TOTAL REVENUE DAILY: %.2lf\n", totalShip(*headO));
                    break;
                case 1:
                    printf("\n>>> EXPORTING DATA <<<\n");
                    compileReport(headO, headS, "Compile_Report.txt");
                    printf("Report has been exported successfully to 'Compile_Report.txt'.\n");
                    break;
            }

            printf("\n------------------------------------------\n");
            printf("Press any key to return to Statistics Menu.");
            getch();
        }
    }
}
