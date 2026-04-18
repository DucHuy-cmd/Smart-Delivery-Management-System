#include<ctype.h>
#include<stdbool.h>
#include<windows.h>
#include <time.h>

#define MAP_SIZE 30
typedef struct { int x, y; } MapPoint;

char grid[MAP_SIZE][MAP_SIZE];
MapPoint parent[MAP_SIZE][MAP_SIZE];
bool visited[MAP_SIZE][MAP_SIZE];

// Ham dieu phoi don hang cho shipper va xuat file bao cao
void dispatchOrders(order **headO, shipper **headS) {
    // Kiem tra xem co du lieu khong
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] He thong chua co don hang hoac shipper de dieu phoi!\n");
        printf("Press Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }

    // Mo file de ghi ket qua dieu phoi
    FILE *f = fopen("dispatch_report.txt", "w");
    if (f == NULL) {
        printf("\n[Loi] Khong the tao file bao cao!\n");
        return;
    }

    fprintf(f, "======================= BAO CAO DIEU PHOI DON HANG =======================\n");

    shipper *s = *headS;
    int totalDispatched = 0; // Bien dem tong so don da dieu phoi thanh cong

    // Duyet qua tung shipper trong danh sach
    while (s != NULL) {
        // Chi giao cho shipper dang ranh (status == 0)
        if (s->status == 0) {
            double currentLoad = 0.0;
            int count = 0;
            order *batch[100]; // Mang tam luu cac don hang duoc giao cho shipper nay

            // Duyet qua danh sach don hang
            order *o = *headO;
            while (o != NULL) {
                // Dieu kien: 
                // 1. Don chua giao (status == 0)
                // 2. Cung loai (o->priority == s->prioritySP) -> 0: Hoa toc, 1: Thuong
                // 3. Khong vuot qua trong luong toi da cua shipper (s->weight)
                if (o->status == 0 && o->priority == s->prioritySP) {
                    if (currentLoad + o->weight <= s->weight) {
                        currentLoad += o->weight;
                        batch[count++] = o;
                        o->status = 1; // Danh dau don nay da co nguoi nhan
                    }
                }
                o = o->next;
            }

            // Neu shipper nay nhan duoc it nhat 1 don hang
            if (count > 0) {
                s->numberOrder += count; // Cap nhat tong so don cua shipper do
                totalDispatched += count; // Cong vao tong so don cua he thong

                // Ghi thong tin shipper va cac don hang da gom vao file
                fprintf(f, "\n--------------------------------------------------------------------------\n");
                fprintf(f, "Shipper: %s | Ten: %s\n", s->code, s->Name);
                fprintf(f, "Loai: %s | Tai trong: %.2lf / %.2lf kg\n", 
                        (s->prioritySP == 0 ? "Hoa toc" : "Binh thuong"), currentLoad, s->weight);
                fprintf(f, "Danh sach don hang da nhan (%d don):\n", count);
                fprintf(f, "%-15s %-15s %-10s\n", "Ma don", "Khoi luong", "Loai don");

                for (int i = 0; i < count; i++) {
                    fprintf(f, "%-15s %-15.2lf %-10s\n", 
                            batch[i]->code, batch[i]->weight, (batch[i]->priority == 0 ? "Hoa toc" : "Binh thuong"));
                }
            }
        }
        s = s->next; // Chuyen sang xet shipper tiep theo
    }

    fclose(f); // Dong file lai sau khi ghi xong

    // In thong bao ra man hinh console cho dep va TU DONG MO FILE
    if (totalDispatched > 0) {
        printf("\n========================================================\n");
        printf("||   DIEU PHOI THANH CONG %-4d DON HANG!              ||\n", totalDispatched);
        printf("||   Chi tiet da duoc luu vao file: dispatch_report.txt ||\n");
        printf("========================================================\n");
        
        printf("\nDang tu dong mo file bao cao...\n");
        // Lenh nay se goi Notepad cua Windows de mo file len ngay lap tuc
        system("notepad dispatch_report.txt"); 

    } else {
        printf("\n[Thong bao] Hien tai khong co don hang nao phu hop de dieu phoi!\n");
        remove("dispatch_report.txt"); // Xoa file rong neu khong co gi de ghi
    }

    // Dung man hinh cho nguoi dung an Enter roi moi thoat
    printf("\nPress Enter To Return...");
    char c = getch();
    while (c != '\r') {
        c = getch();
    }
}

// Ham in ban do ra man hinh
void displayMap(char* sCode, double curW, double maxW) {
    printf("\033[H"); // Dua con tro ve dau trang de chong nhay man hinh
    printf("\n=== HE THONG MO PHONG LO TRINH SHIPPER ===\n");
    printf("Shipper: [%s] | Tai trong: %.1f/%.1f kg\n", sCode, curW, maxW);
    
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
    for (int i = 0; i < MAP_SIZE; i++) {
        printf("| ");
        for (int j = 0; j < MAP_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("|\n");
    }
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
}

// Thuat toan BFS tim duong ne vat can
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

// Ham dieu khien shipper chay tren ban do
void run_animation(MapPoint start, MapPoint end, char* sCode, double curW, double maxW) {
    if (!find_path(start, end)) return;

    MapPoint path[MAP_SIZE * MAP_SIZE];
    int count = 0; MapPoint p = end;
    
    // Truy nguoc mang parent de lay lo trinh
    while (p.x != -1) { path[count++] = p; p = parent[p.x][p.y]; }
    
    // In hanh trinh tu dau den cuoi
    for (int i = count - 1; i >= 0; i--) {
        int r = path[i].x, c = path[i].y;
        char backup = grid[r][c];
        
        grid[r][c] = '#'; // Hien thi shipper
        displayMap(sCode, curW, maxW);
        Sleep(40); // Toc do di chuyen (dung Sleep cua Windows)
        
        // Hoan tra mat duong
        if (backup == 'O') grid[r][c] = 'O'; // Giu nguyen Kho
        else if (backup == '*' || backup == 'v' || backup == 'x') grid[r][c] = backup; // Giu nguyen ky hieu don
        else grid[r][c] = '='; // De lai vet banh xe y nhu file cua ong
    }
}

// Ham mo phong qua trinh di giao hang co ve ban do
void simulateDeliveryRoute(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] He thong chua co don hang hoac shipper!\n");
        printf("\nPress Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }

    char sCode[10];
    printf("\nNhap ma Shipper de bat dau di giao (VD: SP01): ");
    scanf("%s", sCode);
    while (getchar() != '\n'); 

    // Tim Shipper
    shipper *s = *headS;
    while (s != NULL && strcmp(s->code, sCode) != 0) {
        s = s->next;
    }

    if (s == NULL) {
        printf("\n[Loi] Khong tim thay Shipper mang ma %s!\n", sCode);
        printf("\nPress Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }

    // Gom don cho Shipper
    order *batch[50];
    int count = 0;
    double currentLoad = 0;
    
    order *o = *headO;
    while (o != NULL) {
        if (o->status == 0 && o->priority == s->prioritySP && currentLoad + o->weight <= s->weight) {
            batch[count++] = o;
            currentLoad += o->weight;
            o->status = 1; // Dang giao
        }
        o = o->next;
    }

    if (count == 0) {
        printf("\n[Thong bao] Khong co don hang phu hop hoac qua tai trong!\n");
        printf("\nPress Enter To Return...");
        char c = getch(); while (c != '\r') c = getch();
        return;
    }

    // THIET LAP BAN DO
    for (int i = 0; i < MAP_SIZE; i++) 
        for (int j = 0; j < MAP_SIZE; j++) grid[i][j] = '.';

    // Vat can giong file map2.c
    for (int j = 5; j < 25; j++) grid[10][j] = 'X';
    for (int j = 0; j < 20; j++) grid[20][j] = 'X';
    for (int i = 5; i < 15; i++) grid[i][25] = 'X';

    // Dat Kho tai (0,0) va cac don hang '*' tren ban do
    grid[0][0] = 'O'; 
    for(int i = 0; i < count; i++) {
        if(batch[i]->x >= 0 && batch[i]->x < MAP_SIZE && batch[i]->y >= 0 && batch[i]->y < MAP_SIZE) {
            grid[batch[i]->x][batch[i]->y] = '*';
        }
    }
    // Xoa man hinh lan dau va an con tro chuot cho dep
    printf("\033[2J\033[?25l"); 
    MapPoint currentPos = {0, 0}; // Shipper luon xuat phat tu KHO (0,0)
    int successCount = 0, failCount = 0;
    // BAT DAU DI GIAO TUNG DON
    for (int i = 0; i < count; i++) {
        MapPoint nextDest = {batch[i]->x, batch[i]->y};    
        // Goi ham chay hoat anh tren ban do
        run_animation(currentPos, nextDest, s->code, currentLoad, s->weight); 
        // Random 80% thanh cong, 20% that bai sau khi den noi
        int r = rand() % 100;
        if (r < 80) {
            batch[i]->status = 2; // Giao thanh cong
            grid[nextDest.x][nextDest.y] = 'v'; // Doi '*' thanh 'v'
            successCount++;
        } else {
            batch[i]->status = 3; // Giao that bai
            grid[nextDest.x][nextDest.y] = 'x'; // Doi '*' thanh 'x'
            failCount++;
        }
        // Cap nhat lai ban do de hien thi chu 'v' hoac 'x' vua doi
        displayMap(s->code, currentLoad, s->weight);
        printf("\n=> Don hang %s (%d, %d): %s\n", 
            batch[i]->code, batch[i]->x, batch[i]->y, (batch[i]->status == 2 ? "GIAO THANH CONG [v]" : "KHACH BOM HANG [x]"));

        Sleep(1200); // Dung xiu de nguoi dung xem ket qua
        currentPos = nextDest; // Dat diem xuat phat cho chang tiep theo
    }
    // Hien lai con tro chuot sau khi hoat anh ket thuc
    printf("\033[?25h"); 
    s->x = currentPos.x;
    s->y = currentPos.y;
    s->numberOrder += successCount; 
    // XUAT FILE KET QUA
    FILE *f = fopen("trip_simulation_report.txt", "w");
    if (f) {
        fprintf(f, "================= KET QUA CHUYEN DI (SIMULATION) =================\n");
        fprintf(f, "Shipper: %s | Ten: %s\n", s->code, s->Name);
        fprintf(f, "Tai trong da mang: %.2lf / %.2lf kg\n", currentLoad, s->weight);
        fprintf(f, "Tong ket: Giao thanh cong %d don | That bai %d don\n", successCount, failCount);
        fprintf(f, "------------------------------------------------------------------\n");
        fprintf(f, "%-10s | %-15s | %-10s | %-15s\n", "Ma Don", "Toa do", "Khoi luong", "Trang thai");
        
        for(int i = 0; i < count; i++) {
            fprintf(f, "%-10s | X:%-3d Y:%-3d   | %-10.2lf | %-15s\n", 
                    batch[i]->code, batch[i]->x, batch[i]->y, batch[i]->weight, 
                    (batch[i]->status == 2 ? "Thanh Cong" : "That Bai"));
        }
        fclose(f);
        
        printf("\n>>> HOAN THANH! Dang tu dong mo file bao cao...\n");
        system("notepad trip_simulation_report.txt");
    }

    printf("\nPress Enter To Return...");
    char c = getch(); while (c != '\r') c = getch();
}

// OPTION 3: SMART COORDINATION
int Smart_Coordination(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;
    // Them Option 3 vao text de nguoi dung biet duong thoat
    printf("\n1. Automatically assign orders by priority"
           "\n2. Suggestions for the optimal route for a delivery driver"
           "\n3. Return to Main Menu\n\n"); 
           
    do {
        if (countChoice == 3) {
            return -1; // Nhap sai 3 lan thi khoa he thong
        }       
        printf("Enter your choice(1-3): ");
        scanf("%d", &choiceTwo);
        ++countChoice;
    } while (choiceTwo > 3 || choiceTwo < 1);

    switch (choiceTwo) {
        case 1:
            dispatchOrders(headO, headS); // Chuc nang 1: Gom don va xuat file
            break;
        case 2:
            simulateDeliveryRoute(headO, headS); // Chuc nang 2: Chay xe tren ban do
            break;
        case 3:
            return 0; // Thoat an toan ve menu chinh cua SelectOption
    }
    return 0;
}
