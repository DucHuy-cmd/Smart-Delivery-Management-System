#include "Lib.h"

// Hàm thực hiện điều phối đơn hàng
void dispatchOrders(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[!] System lacks orders or shippers to dispatch!\n");
        return;
    }

    FILE *f = fopen("dispatch_report.txt", "w");
    if (f == NULL) return;

    // Get current time and format with AM/PM
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char timeStr[20];
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &tm);
    strftime(timeStr, sizeof(timeStr), "%I:%M:%S %p", &tm); // %I is 12h, %p is AM/PM

    // --- REPORT HEADER ---
    fprintf(f, "==========================================================================================\n");
    fprintf(f, "                             SMART ORDER DISPATCH SYSTEM                                 \n");
    fprintf(f, "                      Date: %-10s  -  Time: %-12s                        \n", dateStr, timeStr);
    fprintf(f, "==========================================================================================\n\n");

    printf("\n[SYSTEM] Dispatching in progress...\n");

    shipper *s = *headS;
    int totalDispatched = 0;
    double totalWeightDispatched = 0;

    while (s != NULL) {
        if (s->status == 0) { // Shipper is available
            int hasOrder = 0;
            double currentLoad = 0;
            order *o = *headO;

            while (o != NULL) {
                if (o->status == 0 && o->priority == s->prioritySP && (currentLoad + o->weight) <= s->weight) {
                    if (hasOrder == 0) {
                        // Shipper info header in English
                        fprintf(f, "SHIPPER: [%-5s] - Name: %-20s | Type: %-10s | Max Load: %.2f kg\n", 
                                s->code, s->Name, (s->prioritySP == 1 ? "EXPRESS" : "NORMAL"), s->weight);
                        fprintf(f, "  +-------+------------+----------------------+------------+------------+\n");
                        fprintf(f, "  | NO.   | ORDER ID   | CUSTOMER NAME        | WEIGHT     | STATUS     |\n");
                        fprintf(f, "  +-------+------------+----------------------+------------+------------+\n");
                    }
                    
                    o->status = 1; 
                    currentLoad += o->weight;
                    hasOrder++;
                    totalDispatched++;
                    totalWeightDispatched += o->weight;

                    // Table row - Perfectly aligned
                    fprintf(f, "  | %-5d | %-10.10s | %-20.20s | %-7.2f kg | %-10.10s |\n", 
                            hasOrder, o->code, o->customerName, o->weight, "ASSIGNED");
                    
                    printf("[+] Order [%s] -> Shipper [%s]\n", o->code, s->code);
                }
                o = o->next;
            }

            if (hasOrder > 0) {
                s->status = 1; 
                fprintf(f, "  +-------+------------+----------------------+------------+------------+\n");
                fprintf(f, "  >> Actual Payload: %.2f kg\n\n", currentLoad);
                fprintf(f, "------------------------------------------------------------------------------------------\n\n");
            }
        }
        s = s->next;
    }

    // --- FINAL SUMMARY ---
    fprintf(f, "==========================================================================================\n");
    fprintf(f, "                                 DISPATCH SESSION SUMMARY                                \n");
    fprintf(f, "  - Total Orders Assigned:   %d units\n", totalDispatched);
    fprintf(f, "  - Total Weight Delivered:  %.2f kg\n", totalWeightDispatched);
    fprintf(f, "==========================================================================================\n");

    fclose(f);
    printf("\n[SUCCESS] Report exported to 'dispatch_report.txt'!\n");
    
    // Automatically open the report
    system("start notepad dispatch_report.txt");

    printf("Press Enter to return...");
    getch();
}

// OPTION 3 (Smart Coordination)
int Smart_Coordination(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    
    do {
        system("cls"); // Xóa màn hình cho menu gọn gàng
        printf("\n================ SMART COORDINATION ================\n");
        printf("1. Automatically assign orders by priority\n");
        printf("2. Suggestions for the optimal route for a delivery driver\n");
        printf("3. Back to main menu\n");
        printf("====================================================\n\n");
        
        if(countChoice == 3) {
            printf("\n[!] Nhap sai qua nhieu lan. Khoa tinh nang!\n");
            system("pause");
            return -1;
        }        
        
        printf("Enter your choice(1-3): ");
        scanf("%d", &choiceTwo);
        
        switch(choiceTwo) {
            case 1:
                dispatchOrders(headO, headS);
                printf("\nAn Enter de tiep tuc...");
                fflush(stdin); getchar();
                countChoice = 0; // Reset số lần đếm
                break;
            case 2:
                printf("\n[Hien tai chua co tinh nang nay, minh se lam sau...]\n");
                printf("\nAn Enter de tiep tuc...");
                fflush(stdin); getchar();
                countChoice = 0;
                break;
            case 3:
                return 0; // Quay về menu chính
            default:
                printf("\nLua chon khong hop le!\n");
                ++countChoice;
                Sleep(1000); // Tạm dừng 1s cho dễ nhìn
                break;
        }
    } while(choiceTwo != 3);
    
    return 0;
}
