#include "Lib.h"

// Ham tinh tong doanh thu tu cac don hang
double totalShip(order *headO){
    double total = 0;
    if(headO == NULL) return 0.0;
    if(headO->status == 2){
        total = headO->fee;
    }
    return total + totalShip(headO->next);
}

// Ham xuat bao cao thong ke ra file
void compileReport(order **headO, shipper **headS, char file[]){
    FILE *f = fopen(file,"w");
    if(f == NULL){
        setColor(12, 0);
        printf("\nERROR: CANNOT OPEN FILE!");
        setColor(15, 0);
        return;
    }

    // Khung chuan 108 ky tu (inner = 106), giong voi Shipper_information.txt
    #define TOP "+==========================================================================================================+\n"
    #define SEP "+----------------------------------------------------------------------------------------------------------+\n"

    fprintf(f, TOP);
    fprintf(f, "|%34s%-37s%35s|\n", "", "SMART DELIVERY - DAILY SUMMARY REPORT", "");
    fprintf(f, TOP);
    fprintf(f, "\n");

    fprintf(f, TOP);
    fprintf(f, "|%43s%-20s%43s|\n", "", "PART I: SHIPPER LIST", "");
    fprintf(f, SEP);
    fprintf(f, "| %-8s | %-44s | %-22s | %-21s |\n", "CODE", "NAME", "CCCD", "STATUS");
    fprintf(f, SEP);

    shipper *tempS = *headS;
    while (tempS != NULL) {
        char *statusStr = (tempS->status == 0) ? "Available" : "Busy";
        fprintf(f, "| %-8s | %-44s | %-22lld | %-21s |\n",
                tempS->code, tempS->Name, tempS->CCCD, statusStr);
        tempS = tempS->next;
    }
    fprintf(f, TOP);
    fprintf(f, "\n");

    fprintf(f, TOP);
    fprintf(f, "|%43s%-19s%44s|\n", "", "PART II: ORDER LIST", "");
    fprintf(f, SEP);
    fprintf(f, "| %-8s | %-29s | %-14s | %-16s | %-25s |\n",
            "CODE", "CUSTOMER NAME", "WEIGHT(kg)", "STATUS", "FEE");
    fprintf(f, SEP);

    order *tempO = *headO;
    while (tempO != NULL) {
        char *statusStr;
        double fee;
        if      (tempO->status == 0) { statusStr = "Pending";   fee = 0.0; }
        else if (tempO->status == 1) { statusStr = "Shipping";  fee = 0.0; }
        else                         { statusStr = "Delivered"; fee = tempO->fee; }
        fprintf(f, "| %-8s | %-29s | %-14.2f | %-16s | %-25.2f |\n",
                tempO->code, tempO->customerName, tempO->weight, statusStr, fee);
        tempO = tempO->next;
    }
    fprintf(f, TOP);
    fprintf(f, "\n");

    double total = totalShip(*headO);
    char revStr[60];
    sprintf(revStr, "TOTAL REVENUE EARNED:  %.2f VND", total);
    int rLen  = (int)strlen(revStr);
    int rLeft = (106 - rLen) / 2;
    int rRight= 106 - rLen - rLeft;
    fprintf(f, TOP);
    fprintf(f, "|%41s%-23s%42s|\n", "", "PART III: TOTAL REVENUE", "");
    fprintf(f, SEP);
    fprintf(f, "|%*s%s%*s|\n", rLeft, "", revStr, rRight, "");
    fprintf(f, TOP);

    #undef TOP
    #undef SEP

    fclose(f);
    setColor(10, 0);
    printf("DATA HAS BEEN SUCCESSFULLY WRITTEN TO FILE %s!", file);
    setColor(15, 0);
}

// Ham ve giao dien Menu Thong ke bao cao
void Draw_StatisticsMenu(int pointer) {
    char *options[] = {
        "Calculate total daily revenue",
        "Export report (Compile_Report.txt)",
        "BACK TO MAIN MENU"
    };
    char buf[MENU_W + 2];
    drawHLine(0);
    drawTitleRow("[ 4 ]   STATISTICS AND REPORTS");
    drawHLine(1);
    drawEmptyRow();
    for (int i = 0; i < 3; i++) {
        if (i == pointer)
            sprintf(buf, "  > [ %d ]. %s", (i == 2 ? 0 : i + 1), options[i]);
        else
            sprintf(buf, "    [ %d ]. %s", (i == 2 ? 0 : i + 1), options[i]);
        drawItemRow(buf, i == pointer);
    }
    drawEmptyRow();
    drawHLine(1);
    drawHintRow("    [^][v] Navigate   |   [ENTER] Select");
    drawHLine(2);
}

// Ham xu ly logic chinh cua Thong ke va Bao cao
int Statistics_and_Reports(order **headO, shipper **headS) {
    int pointer = 0;
    char key;

    system("cls");
    while(1) {
        goHome(); Draw_StatisticsMenu(pointer);

        key = getch();

        if (key == -32) {
            key = getch();
            if (key == 72) {
                if (pointer > 0) pointer--;
                else pointer = 2;
            } else if (key == 80) {
                if (pointer < 2) pointer++;
                else pointer = 0;
            }
        }
        else if (key == 13) {
            system("cls");
            if (pointer == 2) return 0;

            switch (pointer) {
                case 0:
                    setColor(11, 0);
                    printf("\n>>> REVENUE CALCULATION <<<\n");
                    setColor(15, 0);
                    printf("TOTAL REVENUE DAILY: ");
                    setColor(10, 0);
                    printf("%.2lf VND\n", totalShip(*headO));
                    setColor(15, 0);
                    break;
                case 1:
                    setColor(14, 0);
                    printf("\n>>> EXPORTING DATA <<<\n");
                    setColor(15, 0);
                    compileReport(headO, headS, "Compile_Report.txt");
                    setColor(10, 0);
                    printf("\nReport has been exported successfully to 'Compile_Report.txt'.\n");
                    setColor(15, 0);
                    break;
            }

            setColor(11, 0);
            printf("\n------------------------------------------\n");
            setColor(8, 0);
            printf("Press any key to return to Statistics Menu.");
            setColor(15, 0);
            getch();
            system("cls");
        }
    }
}
