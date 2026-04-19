#include <time.h>

#define MAP_SIZE 30
typedef struct { int x, y; } MapPoint;

char grid[MAP_SIZE][MAP_SIZE];
MapPoint parent[MAP_SIZE][MAP_SIZE];
bool visited[MAP_SIZE][MAP_SIZE];

void dispatchOrders(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] He thong chua co don hang hoac shipper de dieu phoi!\n");
        printf("Press Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }
    FILE *f = fopen("dispatch_report.txt", "w");
    if (f == NULL) return;

    fprintf(f, "======================= BAO CAO DIEU PHOI DON HANG =======================\n");
    shipper *s = *headS;
    int totalDispatched = 0; 
    while (s != NULL) {
        if (s->status == 0) {
            double currentLoad = 0.0;
            int count = 0;
            order *batch[100]; 
            order *o = *headO;
            while (o != NULL) {
                if (o->status == 0 && o->priority == s->prioritySP) {
                    if (currentLoad + o->weight <= s->weight) {
                        currentLoad += o->weight;
                        batch[count++] = o;
                        o->status = 1; 
                    }
                }
                o = o->next;
            }
            if (count > 0) {
                s->numberOrder += count; 
                totalDispatched += count; 
                fprintf(f, "\n--------------------------------------------------------------------------\n");
                fprintf(f, "Shipper: %s | Ten: %s\n", s->code, s->Name);
                fprintf(f, "Loai: %s | Tai trong: %.2lf / %.2lf kg\n", (s->prioritySP == 0 ? "Hoa toc" : "Binh thuong"), currentLoad, s->weight);
                fprintf(f, "Danh sach don hang da nhan (%d don):\n", count);
                fprintf(f, "%-15s %-15s %-10s\n", "Ma don", "Khoi luong", "Loai don");
                for (int i = 0; i < count; i++) {
                    fprintf(f, "%-15s %-15.2lf %-10s\n", batch[i]->code, batch[i]->weight, (batch[i]->priority == 0 ? "Hoa toc" : "Binh thuong"));
                }
            }
        }
        s = s->next; 
    }
    fclose(f); 

    informationShipper(headS);

    if (totalDispatched > 0) {
        printf("\n========================================================\n");
        printf("||   DIEU PHOI THANH CONG %-4d DON HANG!                ||\n", totalDispatched);
        printf("||   Chi tiet da duoc luu vao file: dispatch_report.txt ||\n");
        printf("========================================================\n");
        printf("\nDa thong ke vao file bao cao...\n");
        system("notepad dispatch_report.txt"); 
    } else {
        printf("\n[Thong bao] Hien tai khong co don hang nao phu hop de dieu phoi!\n");
        remove("dispatch_report.txt"); 
    }
    printf("\nPress Enter To Return...");
    char c = getch(); while (c != '\r') c = getch();
}

void displayMap(char* sCode, double curW, double maxW) {
    printf("\033[H"); 
    printf("\n=== HE THONG MO PHONG LO TRINH SHIPPER ===\n");
    printf("Shipper: [%s] | Tai trong: %.1f/%.1f kg\n", sCode, curW, maxW);
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
    for (int i = 0; i < MAP_SIZE; i++) {
        printf("| ");
        for (int j = 0; j < MAP_SIZE; j++) printf("%c ", grid[i][j]);
        printf("|\n");
    }
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
}

bool find_path(MapPoint start, MapPoint end) {
    for(int i=0; i<MAP_SIZE; i++) for(int j=0; j<MAP_SIZE; j++) {
        visited[i][j] = false;
        parent[i][j] = (MapPoint){-1, -1};
    }
    MapPoint queue[MAP_SIZE * MAP_SIZE];
    int head = 0, tail = 0;
    queue[tail++] = start;
    visited[start.x][start.y] = true;
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    while (head < tail) {
        MapPoint curr = queue[head++];
        if (curr.x == end.x && curr.y == end.y) return true;
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < MAP_SIZE && ny >= 0 && ny < MAP_SIZE && !visited[nx][ny] && grid[nx][ny] != 'X') {
                visited[nx][ny] = true;
                parent[nx][ny] = curr;
                queue[tail++] = (MapPoint){nx, ny};
            }
        }
    }
    return false;
}

void run_animation(MapPoint start, MapPoint end, char* sCode, double curW, double maxW) {
    if (!find_path(start, end)) return;
    MapPoint path[MAP_SIZE * MAP_SIZE];
    int count = 0; MapPoint p = end;
    while (p.x != -1) { path[count++] = p; p = parent[p.x][p.y]; }
    for (int i = count - 1; i >= 0; i--) {
        int r = path[i].x, c = path[i].y;
        char backup = grid[r][c];
        grid[r][c] = '#'; 
        displayMap(sCode, curW, maxW);
        Sleep(40); 
        if (backup == 'O') grid[r][c] = 'O'; 
        else if (backup == '*' || backup == 'v' || backup == 'x') grid[r][c] = backup; 
        else grid[r][c] = '='; 
    }
}

void simulateDeliveryRoute(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] He thong chua co du lieu de mo phong!\n");
        printf("Press Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }

    // 1. Mo file bao cao mot lan duy nhat o dau ham
    FILE *f = fopen("trip_simulation_report.txt", "w");
    if (f == NULL) return;
    fprintf(f, "======================= BAO CAO HANH TRINH GIAO HANG =======================\n");

    shipper *s = *headS;
    int shipperCount = 0;

    // 2. Vong lap duyet qua tung Shipper
    while (s != NULL) {
        order *batch[100];
        int count = 0;
        double currentLoad = 0.0;
        int successCount = 0, failCount = 0;

        // Gom cac don hang da duoc dieu phoi cho shipper nay (status == 1)
        order *o = *headO;
        while (o != NULL) {
            if (o->status == 1 && o->priority == s->prioritySP) {
                batch[count++] = o;
                currentLoad += o->weight;
            }
            o = o->next;
        }

        // Neu shipper nay co don thi moi bat dau ve Map
        if (count > 0) {
            shipperCount++;
            system("cls"); // Xoa man hinh de chuan bi cho Shipper moi
            printf("====================================================\n");
            printf(">>> KHOI HANH CHUYEN XE CUA SHIPPER: %-10s <<<\n", s->code);
            printf(">>> Loai hinh: %-15s                <<<\n", (s->prioritySP == 1 ? "HOA TOC" : "THUONG"));
            printf("====================================================\n");
            Sleep(1500); // Dung 1.5s de nguoi dung kip doc ten Shipper

            MapPoint currentPos = {0, 0}; // Xuat phat tu kho (0,0)

            for (int i = 0; i < count; i++) {
                MapPoint target = {batch[i]->x, batch[i]->y};
                
                if (find_path(currentPos, target)) {
                    run_animation(currentPos, target, s->code, currentLoad, s->weight);
                    batch[i]->status = 2; // Giao thanh cong
                    successCount++;
                    currentPos = target; // Vi tri hien tai tro thanh diem bat dau tiep theo
                } else {
                    batch[i]->status = 3; // That bai (khong tim thay duong)
                    failCount++;
                }
            }

            // Quay tro lai kho (0,0) sau khi giao xong
            MapPoint warehouse = {0, 0};
            if (find_path(currentPos, warehouse)) {
                run_animation(currentPos, warehouse, s->code, currentLoad, s->weight);
            }

            // Cap nhat thanh tich cho Shipper
            s->numberOrder += successCount;
            
            // Ghi du lieu vao file bao cao cho Shipper nay
            fprintf(f, "Shipper: %s (%s)\n", s->code, s->Name);
            fprintf(f, "Trong tai: %.2lf / %.2lf kg\n", currentLoad, s->weight);
            fprintf(f, "Ket qua: Thanh cong %d | That bai %d\n", successCount, failCount);
            fprintf(f, "------------------------------------------------------------\n");
            for(int i = 0; i < count; i++) {
                fprintf(f, "%-10s | Toa do: (%-2d,%-2d) | Trang thai: %s\n", 
                        batch[i]->code, batch[i]->x, batch[i]->y, 
                        (batch[i]->status == 2 ? "THANH CONG" : "THAT BAI"));
            }
            fprintf(f, "============================================================\n\n");

            // Thong bao hoan thanh luot cua Shipper nay
            printf("\n>>> SHIPPER %s DA HOAN THANH LO TRINH!", s->code);
            printf("\nAn Enter de tiep tuc...");
            char key = getch(); while (key != '\r') key = getch();
        }

        s = s->next; // Chuyen sang Shipper tiep theo trong danh sach
    }

    // 3. Ket thuc: Dong file va mo Notepad
    fclose(f);
    
    if (shipperCount > 0) {
        printf("\n\n>>> TAT CA SHIPPER DA HOAN THANH NHIEM VU! <<<\n");
        printf("Dang mo file tong ket hanh trinh...\n");
        system("notepad trip_simulation_report.txt");
    } else {
        printf("\n[Luu y] Khong co shipper nao co don hang de giao!\n");
    }

    printf("\nPress Enter To Return to Main Menu...");
    char c = getch(); while (c != '\r') c = getch();
}

int Smart_Coordination(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    
    do {
        system("cls");
        printf("\n================ SMART COORDINATION ================\n");
        printf("1. Automatically assign orders (Dispatch)\n");
        printf("2. Simulate delivery routes (Run Map)\n");
        printf("3. Return to Main Menu\n");
        printf("====================================================\n\n");
        
        if (countChoice == 3) {
            printf("Nhap sai qua nhieu lan. Dang quay lai Menu chinh...\n");
            Sleep(2000);
            return -1;
        }
        
        printf("Enter your choice: ");
        if (scanf("%d", &choiceTwo) != 1) {
            while(getchar() != '\n');
            choiceTwo = 0;
        }
        while(getchar() != '\n'); // Clean buffer
        
        switch(choiceTwo) {
            case 1:
                // Chức năng điều phối đơn cho shipper
                dispatchOrders(headO, headS);
                break;
            case 2:
                // Chức năng chạy map mô phỏng (hàm mình vừa sửa lúc nãy)
                simulateDeliveryRoute(headO, headS);
                break;
            case 3:
                // Thoát ra Menu chính
                return 1; 
            default:
                printf("Invalid choice! Vui long nhap lai.\n");
                countChoice++;
                Sleep(1000);
        }
    } while(choiceTwo != 3);
    
    return 1;
}
