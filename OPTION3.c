#include "Lib.h"

// Hàm thực hiện điều phối đơn hàng
void dispatchOrders(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] He thong chua co don hang hoac chua co shipper de dieu phoi!\n");
        return;
    }

    FILE *f = fopen("dispatch_report.txt", "w");
    if (f == NULL) {
        printf("\n[Loi] Khong the tao file bao cao!\n");
        return;
    }

    // In Header của Report
    fprintf(f, "====================================== DISPATCH REPORT ======================================\n");
    fprintf(f, "---------------------------------------------------------------------------------------------\n");
    fprintf(f, "%-10s | %-20s | %-12s | %-15s | %-25s\n", "ID SHIPPER", "TEN SHIPPER", "LOAI SHIPPER", "TAI TRONG (kg)", "DON HANG DUOC GAN (ID)");
    fprintf(f, "---------------------------------------------------------------------------------------------\n");

    shipper *s = *headS;
    int totalDispatched = 0; // Đếm tổng số đơn đã điều phối thành công

    while (s != NULL) {
        if (s->status == 0) { // Nếu shipper đang rảnh
            double currentLoad = 0.0;
            int count = 0;
            order *batch[50]; // Giả sử 1 shipper nhận tối đa 50 đơn cùng lúc

            order *o = *headO;
            while (o != NULL) {
                // Điều kiện: Đơn chưa giao (0) VÀ cùng độ ưu tiên (0=Thường, 1=Hỏa tốc)
                if (o->status == 0 && o->priority == s->prioritySP) {
                    // Kiểm tra xem sức chứa của shipper còn đủ để chở thêm không
                    if (currentLoad + o->weight <= s->weight) {
                        currentLoad += o->weight;
                        batch[count++] = o;
                        o->status = 1; // Đánh dấu đơn hàng đang được giao
                    }
                }
                o = o->next;
            }

            // Nếu shipper nhận được đơn
            if (count > 0) {
                s->status = 1; // Đánh dấu shipper đang bận đi giao
                s->numberOrder += count;
                totalDispatched += count;

                // Nối các mã code đơn hàng lại để in cho đẹp
                char assignedCodes[256] = "";
                for (int i = 0; i < count; i++) {
                    strcat(assignedCodes, batch[i]->code);
                    if (i < count - 1) strcat(assignedCodes, ", ");
                }

                char typeStr[15];
                strcpy(typeStr, s->prioritySP == 1 ? "Hoa toc (1)" : "Thuong (0)");

                // Ghi vào file
                fprintf(f, "%-10s | %-20s | %-12s | %-15.2lf | %-25s\n", 
                        s->code, s->Name, typeStr, s->weight, assignedCodes);
            }
        }
        s = s->next; // Chuyển sang shipper tiếp theo
    }

    if (totalDispatched == 0) {
        fprintf(f, "\nKhong co don hang nao duoc dieu phoi (Co the do lech priority hoac da het don/shipper).\n");
    }

    fprintf(f, "---------------------------------------------------------------------------------------------\n");
    fprintf(f, "=> TONG SO DON HANG DA GIAO CHO SHIPPER LA: %d\n", totalDispatched);
    fprintf(f, "=============================================================================================\n");
    fclose(f);

    printf("\n>>> DIEU PHOI THANH CONG! Dang mo file bao cao...\n");
    
    // Mở file tự động (Dùng cho Windows)
    system("start dispatch_report.txt"); 
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
