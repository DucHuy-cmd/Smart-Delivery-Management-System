#include "Lib.h"

// ===================== CẤU HÌNH MAP =====================
#define ROWS 15
#define COLS 30
#define WAREHOUSE_X 0
#define WAREHOUSE_Y 0

// Ký hiệu trên map
#define CELL_EMPTY   '.'
#define CELL_WALL    '#'
#define CELL_PATH    '='
#define CELL_START   'W'   // Kho xuất phát
#define CELL_END     'X'   // Điểm giao hàng
#define CELL_SHIPPER 'S'   // Vị trí shipper

// ===================== STRUCT HỖ TRỢ BFS =====================
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point pt;
    int dist;
} BFSNode;

// ===================== BFS TÌM ĐƯỜNG ĐI NGẮN NHẤT =====================
// Trả về độ dài đường đi, -1 nếu không tìm được
// path[] lưu các điểm trên đường đi
int bfs(char map[ROWS][COLS], int startX, int startY, int endX, int endY, Point path[], int *pathLen) {
    bool visited[ROWS][COLS];
    Point parent[ROWS][COLS];
    memset(visited, 0, sizeof(visited));
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            parent[i][j].x = parent[i][j].y = -1;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    BFSNode queue[ROWS * COLS];
    int front = 0, back = 0;
    queue[back].pt.x = startX;
    queue[back].pt.y = startY;
    queue[back].dist = 0;
    back++;
    visited[startX][startY] = true;

    int found = -1;
    while (front < back) {
        BFSNode cur = queue[front++];
        if (cur.pt.x == endX && cur.pt.y == endY) {
            found = cur.dist;
            break;
        }
        for (int d = 0; d < 4; d++) {
            int nx = cur.pt.x + dx[d];
            int ny = cur.pt.y + dy[d];
            if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLS
                && !visited[nx][ny] && map[nx][ny] != CELL_WALL) {
                visited[nx][ny] = true;
                parent[nx][ny].x = cur.pt.x;
                parent[nx][ny].y = cur.pt.y;
                queue[back].pt.x = nx;
                queue[back].pt.y = ny;
                queue[back].dist = cur.dist + 1;
                back++;
            }
        }
    }

    if (found == -1) return -1;

    // Trace back đường đi
    *pathLen = 0;
    Point temp[ROWS * COLS];
    int tx = endX, ty = endY;
    while (tx != -1 && ty != -1) {
        temp[(*pathLen)].x = tx;
        temp[(*pathLen)].y = ty;
        (*pathLen)++;
        Point p = parent[tx][ty];
        tx = p.x;
        ty = p.y;
    }
    // Đảo ngược để từ start -> end
    for (int i = 0; i < *pathLen; i++)
        path[i] = temp[*pathLen - 1 - i];

    return found;
}

// ===================== IN MAP RA MÀN HÌNH =====================
void printMap(char map[ROWS][COLS], int startX, int startY, int endX, int endY,
              const char *shipperCode, const char *shipperName,
              const char *orderCode, const char *customerName,
              double weight, int priority, int status) {

    // Thông tin đơn hàng và shipper phía trên map
    printf("\n");
    printf("  +====================== SMART DELIVERY MAP ======================+\n");
    printf("  | SHIPPER : [%-5s] %-20s                    |\n", shipperCode, shipperName);
    printf("  | ORDER   : [%-5s] %-20s | KG: %-6.2f       |\n", orderCode, customerName, weight);
    printf("  | PRIORITY: %-10s | STATUS: %-25s|\n",
           priority == 1 ? "EXPRESS" : "NORMAL",
           status == 0 ? "Pending" : (status == 1 ? "Shipping..." : "Delivered"));
    printf("  | ROUTE   : W(%d,%d) ========> X(%d,%d)                         |\n",
           startY, startX, endY, endX);
    printf("  +=================================================================+\n");

    // In khung map
    printf("  +");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");

    for (int i = 0; i < ROWS; i++) {
        printf("  |");
        for (int j = 0; j < COLS; j++) {
            char c = map[i][j];
            printf("%c", c);
        }
        printf("|\n");
    }

    printf("  +");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");

    // Chú thích
    printf("\n");
    printf("  [ CHU THICH / LEGEND ]\n");
    printf("  %-4c = Kho xuat phat (Warehouse)       %-4c = Diem giao hang (Destination)\n", CELL_START, CELL_END);
    printf("  %-4c = Duong di ngan nhat (Short Path)  %-4c = O trong (Empty cell)\n", CELL_PATH, CELL_EMPTY);
    printf("  %-4c = Tuong / Vat can (Wall/Obstacle)  %-4c = Vi tri Shipper\n", CELL_WALL, CELL_SHIPPER);
    printf("\n");
}

// ===================== TẠO MAP NGẪU NHIÊN CÓ TƯỜNG =====================
void generateMap(char map[ROWS][COLS], int endX, int endY) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            map[i][j] = CELL_EMPTY;

    // Tạo tường ngẫu nhiên (~15% ô)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Không đặt tường tại kho và điểm giao
            if ((i == WAREHOUSE_X && j == WAREHOUSE_Y) || (i == endX && j == endY))
                continue;
            if (rand() % 100 < 15)
                map[i][j] = CELL_WALL;
        }
    }
}

// ===================== VẼ ĐƯỜNG ĐI LÊN MAP =====================
void drawPath(char map[ROWS][COLS], Point path[], int pathLen, int startX, int startY, int endX, int endY) {
    for (int i = 0; i < pathLen; i++) {
        int x = path[i].x;
        int y = path[i].y;
        if ((x == startX && y == startY) || (x == endX && y == endY))
            continue;
        map[x][y] = CELL_PATH;
    }
    map[startX][startY] = CELL_START;
    map[endX][endY]     = CELL_END;
}

// ===================== LƯU TRẠNG THÁI VÀO FILE =====================
void saveDeliveryResult(order **headO, shipper **headS,
                        const char *orderCode, const char *shipperCode,
                        int newOrderStatus, int newShipperStatus) {
    // Cập nhật trong linked list
    order *o = *headO;
    while (o != NULL) {
        if (strcmp(o->code, orderCode) == 0) {
            o->status = newOrderStatus;
            o->isSaved = 0; // Đánh dấu cần lưu lại
            break;
        }
        o = o->next;
    }
    shipper *s = *headS;
    while (s != NULL) {
        if (strcmp(s->code, shipperCode) == 0) {
            s->status = newShipperStatus;
            break;
        }
        s = s->next;
    }

    // Ghi toàn bộ file order lại
    FILE *f = fopen("Order_Information.txt", "w");
    if (f == NULL) { printf("\n[!] Khong the mo file Order_Information.txt de luu!\n"); return; }

    fprintf(f, "%-5s || %-20s || %-20s || %-3s || %-3s || %-6s || %-10s || %-10s || %-10s || %-10s\n",
            "Code", "Order Name", "Customer Name", "X", "Y", "Weight", "Date", "Fee", "Priority", "Status");

    char *StatusStr[] = {"Pending", "Shipping", "Delivered", "Failed"};
    o = *headO;
    while (o != NULL) {
        char *st = (o->status >= 0 && o->status <= 3) ? StatusStr[o->status] : "Unknown";
        fprintf(f, "%-5s || %-20s || %-20s || %-3d || %-3d || %-6.2lf || %02d/%02d/%04d || %-10.2lf || %-10s || %-10s\n",
                o->code, o->orderName, o->customerName,
                o->x, o->y, o->weight,
                o->date.day, o->date.month, o->date.year,
                o->fee,
                (o->priority == 1) ? "Express" : "Normal",
                st);
        o->isSaved = 1;
        o = o->next;
    }
    fclose(f);

    // Ghi file shipper
    FILE *fs = fopen("Shipper_Information.txt", "w");
    if (fs != NULL) {
        fprintf(fs, "====================================List shipper==================================\n");
        fprintf(fs, "----------------------------------------------------------------------------------\n");
        fprintf(fs, "||%-30s||%-20s||%-10s||%-10s||%-10s||\n", "Name", "CCCD", "CODE", "Order", "Status");
        s = *headS;
        while (s != NULL) {
            fprintf(fs, "||%-30s||%-20lld||%-10s||%-10d||%-10s||\n",
                    s->Name, s->CCCD, s->code, s->numberOrder,
                    (s->status == 0) ? "Available" : "Busy");
            s = s->next;
        }
        fclose(fs);
    }
}

// ===================== CHỨC NĂNG GỢI Ý ĐƯỜNG ĐI TỐI ƯU =====================
void suggestOptimalRoute(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[!] Khong co don hang hoac shipper trong he thong!\n");
        printf("Press any key to return...");
        getch();
        return;
    }

    // Hiển thị danh sách shipper đang rảnh
    printf("\n============ DANH SACH SHIPPER TRONG (AVAILABLE) ============\n");
    shipper *s = *headS;
    int countS = 0;
    while (s != NULL) {
        if (s->status == 0) {
            printf("  [%d] Code: %-5s | Ten: %-20s | Loai: %s\n",
                   ++countS, s->code, s->Name, s->prioritySP == 1 ? "Express" : "Normal");
        }
        s = s->next;
    }
    if (countS == 0) {
        printf("  [!] Khong co shipper nao dang trong!\n");
        printf("Press any key to return..."); getch();
        return;
    }

    // Chọn shipper
    char shipCode[10];
    printf("\nNhap ma shipper can tim duong: ");
    scanf("%s", shipCode);
    shipper *chosenShipper = NULL;
    s = *headS;
    while (s != NULL) {
        if (strcmp(s->code, shipCode) == 0 && s->status == 0) {
            chosenShipper = s;
            break;
        }
        s = s->next;
    }
    if (chosenShipper == NULL) {
        printf("\n[!] Khong tim thay shipper hoac shipper dang ban!\n");
        printf("Press any key to return..."); getch();
        return;
    }

    // Hiển thị danh sách đơn hàng Pending phù hợp
    printf("\n============ DON HANG CHO GIAO (PENDING - PHU HOP) ============\n");
    order *o = *headO;
    int countO = 0;
    while (o != NULL) {
        if (o->status == 0 && o->priority == chosenShipper->prioritySP
            && o->weight <= chosenShipper->weight) {
            printf("  [%d] Code: %-5s | KH: %-20s | KG: %.2f | Toa do: (%d,%d)\n",
                   ++countO, o->code, o->customerName, o->weight, o->x, o->y);
        }
        o = o->next;
    }
    if (countO == 0) {
        printf("  [!] Khong co don hang phu hop cho shipper nay!\n");
        printf("Press any key to return..."); getch();
        return;
    }

    // Chọn đơn hàng
    char orderCode[10];
    printf("\nNhap ma don hang can giao: ");
    scanf("%s", orderCode);
    order *chosenOrder = NULL;
    o = *headO;
    while (o != NULL) {
        if (strcmp(o->code, orderCode) == 0 && o->status == 0) {
            chosenOrder = o;
            break;
        }
        o = o->next;
    }
    if (chosenOrder == NULL) {
        printf("\n[!] Khong tim thay don hang hoac don hang khong hop le!\n");
        printf("Press any key to return..."); getch();
        return;
    }

    // Kiểm tra tọa độ trong phạm vi map
    int endX = chosenOrder->x % ROWS;
    int endY = chosenOrder->y % COLS;
    if (endX < 0) endX = 0;
    if (endY < 0) endY = 0;
    // Đảm bảo không trùng kho
    if (endX == WAREHOUSE_X && endY == WAREHOUSE_Y) endX = 1;

    // Tạo map
    char map[ROWS][COLS];
    generateMap(map, endX, endY);

    // BFS tìm đường
    Point path[ROWS * COLS];
    int pathLen = 0;
    int retry = 0;
    int dist = -1;

    // Thử lại tối đa 5 lần nếu map ngẫu nhiên chặn đường
    while (dist == -1 && retry < 5) {
        generateMap(map, endX, endY);
        dist = bfs(map, WAREHOUSE_X, WAREHOUSE_Y, endX, endY, path, &pathLen);
        retry++;
    }

    system("cls");

    if (dist == -1) {
        printf("\n[!] Khong tim duoc duong di toi diem giao hang! (Bi chan boi tuong)\n");
        printf("    Toa do dich: (%d, %d)\n", endY, endX);
        printf("Press any key to return..."); getch();
        return;
    }

    // Vẽ đường lên map
    drawPath(map, path, pathLen, WAREHOUSE_X, WAREHOUSE_Y, endX, endY);

    // Cập nhật trạng thái: đang giao
    chosenOrder->status = 1;
    chosenShipper->status = 1;
    chosenShipper->numberOrder++;

    // In map + thông tin
    printMap(map, WAREHOUSE_X, WAREHOUSE_Y, endX, endY,
             chosenShipper->code, chosenShipper->Name,
             chosenOrder->code, chosenOrder->customerName,
             chosenOrder->weight, chosenOrder->priority, chosenOrder->status);

    printf("  Khoang cach duong di: %d buoc\n", dist);
    printf("  Phi ship uoc tinh : %.2f VND\n", chosenOrder->fee);
    printf("\n  [SHIPPING] Dang giao hang...\n");
    printf("  Nhan phim bat ky de ket thuc giao hang...");
    getch();

    // ===== Random kết quả giao hàng: 80% thành công, 20% thất bại =====
    srand((unsigned)time(NULL) + (unsigned)chosenOrder->x + (unsigned)chosenOrder->y);
    int roll = rand() % 100;
    int deliverySuccess = (roll < 80); // 80% thành công

    system("cls");

    // In lại map với trạng thái mới
    int finalStatus = deliverySuccess ? 2 : 0; // 2=Delivered, 0=Pending lại
    chosenOrder->status = finalStatus;
    chosenShipper->status = 0; // Shipper rảnh trở lại

    printMap(map, WAREHOUSE_X, WAREHOUSE_Y, endX, endY,
             chosenShipper->code, chosenShipper->Name,
             chosenOrder->code, chosenOrder->customerName,
             chosenOrder->weight, chosenOrder->priority, finalStatus);

    if (deliverySuccess) {
        printf("  +=================================================================+\n");
        printf("  |  [SUCCESS] GIAO HANG THANH CONG!                                |\n");
        printf("  |  Don hang [%-5s] da duoc giao toi khach hang.                   |\n", chosenOrder->code);
        printf("  |  Trang thai don hang: DELIVERED                                 |\n");
        printf("  |  Trang thai shipper : AVAILABLE (da giao xong)                  |\n");
        printf("  +=================================================================+\n");
    } else {
        printf("  +=================================================================+\n");
        printf("  |  [FAILED] GIAO HANG THAT BAI!                                   |\n");
        printf("  |  Don hang [%-5s] khong giao duoc (khach vang mat, sai dia chi)  |\n", chosenOrder->code);
        printf("  |  Trang thai don hang: PENDING (tra ve kho)                      |\n");
        printf("  |  Trang thai shipper : AVAILABLE (da quay ve)                    |\n");
        printf("  +=================================================================+\n");
    }

    // Lưu kết quả vào file
    saveDeliveryResult(headO, headS,
                       chosenOrder->code, chosenShipper->code,
                       finalStatus, 0);

    printf("\n  [FILE] Da luu trang thai vao Order_Information.txt & Shipper_Information.txt\n");
    printf("  Dang mo file...\n");

    // Tự động mở file
    Sleep(1000);
    system("start notepad Order_Information.txt");
    Sleep(500);
    system("start notepad Shipper_Information.txt");

    printf("\nPress any key to return to menu...");
    getch();
}

// ===================== DISPATCH TỰ ĐỘNG (GIỮ NGUYÊN) =====================
void dispatchOrders(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[!] System lacks orders or shippers to dispatch!\n");
        return;
    }

    FILE *f = fopen("dispatch_report.txt", "w");
    if (f == NULL) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char timeStr[20], dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &tm);
    strftime(timeStr, sizeof(timeStr), "%I:%M:%S %p", &tm);

    fprintf(f, "==========================================================================================\n");
    fprintf(f, "                             SMART ORDER DISPATCH SYSTEM                                 \n");
    fprintf(f, "                      Date: %-10s  -  Time: %-12s                        \n", dateStr, timeStr);
    fprintf(f, "==========================================================================================\n\n");

    printf("\n[SYSTEM] Dispatching in progress...\n");

    shipper *s = *headS;
    int totalDispatched = 0;
    double totalWeightDispatched = 0;

    while (s != NULL) {
        if (s->status == 0) {
            int hasOrder = 0;
            double currentLoad = 0;
            order *o = *headO;

            while (o != NULL) {
                if (o->status == 0 && o->priority == s->prioritySP && (currentLoad + o->weight) <= s->weight) {
                    if (hasOrder == 0) {
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

    fprintf(f, "==========================================================================================\n");
    fprintf(f, "                                 DISPATCH SESSION SUMMARY                                \n");
    fprintf(f, "  - Total Orders Assigned:   %d units\n", totalDispatched);
    fprintf(f, "  - Total Weight Delivered:  %.2f kg\n", totalWeightDispatched);
    fprintf(f, "==========================================================================================\n");

    fclose(f);
    printf("\n[SUCCESS] Report exported to 'dispatch_report.txt'!\n");
    system("start notepad dispatch_report.txt");
    printf("Press Enter to return...");
    getch();
}

// ===================== OPTION 3 - SMART COORDINATION =====================
int Smart_Coordination(order **headO, shipper **headS) {
    int countChoice = 0;
    int choiceTwo;

    do {
        system("cls");
        printf("\n================ SMART COORDINATION ================\n");
        printf("  1. Tu dong dieu phoi don hang theo uu tien\n");
        printf("  2. Goi y duong di toi uu cho shipper (MAP + BFS)\n");
        printf("  3. Quay lai menu chinh\n");
        printf("====================================================\n\n");

        if (countChoice == 3) {
            printf("\n[!] Nhap sai qua nhieu lan. Khoa tinh nang!\n");
            system("pause");
            return -1;
        }

        printf("Enter your choice(1-3): ");
        scanf("%d", &choiceTwo);

        switch (choiceTwo) {
            case 1:
                dispatchOrders(headO, headS);
                countChoice = 0;
                break;
            case 2:
                suggestOptimalRoute(headO, headS);
                countChoice = 0;
                break;
            case 3:
                return 0;
            default:
                printf("\nLua chon khong hop le!\n");
                ++countChoice;
                Sleep(1000);
                break;
        }
    } while (choiceTwo != 3);

    return 0;
}
