#include "Lib.h"

// ================================================================
//  OPTION 3 - SMART COORDINATION
//  [1] Dieu phoi tu dong
//  [2] Animation shipper giao hang (khong nhap nhay, tim don gan nhat)
//  [3] Tong quan kho hang (9 khu 3x3 ###)
//  [4] Goi y duong di (ban do BFS)
//  [5] Quay lai
// ================================================================

// --- KY HIEU BAN DO ---
#define SYM_EMPTY     '.'
#define SYM_WALL      'X'
#define SYM_PATH      '='
#define SYM_WAREHOUSE 'W'
#define SYM_PENDING   'P'
#define SYM_SHIPPING  'G'
#define SYM_DELIVERED 'D'
#define SYM_SHIPPER   'S'

#define WH_ROW 0
#define WH_COL 0

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// BFS 4 huong
static int dRow[] = {-1, 1,  0, 0};
static int dCol[] = { 0, 0, -1, 1};

// Chuong ngai vat
static int wallR[] = { 2, 2, 2,  5, 5, 5,  8, 8, 11, 11, 11 };
static int wallC[] = { 3, 4, 5,  7, 8, 9,  2, 3,  6,  7,  8 };
#define NUM_WALLS 11

static int clampVal(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

static void enableAnsiControl(void) {
    static int initialized = 0;
    if (initialized) return;
    initialized = 1;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, mode)) return;
}

// ================================================================
//  WINDOWS CONSOLE UTILITIES
// ================================================================
static void gotoXY(int row, int col) {
    enableAnsiControl();
    printf("\x1b[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}

static void hideCursor(void) {
    enableAnsiControl();
    printf("\x1b[?25l");
    fflush(stdout);
}

static void showCursor(void) {
    enableAnsiControl();
    printf("\x1b[?25h");
    fflush(stdout);
}

// ================================================================
//  KHOI TAO / DAT DON / DAT SHIPPER
// ================================================================
static void initMap(char map[][MAP_SIZE]) {
    for (int r = 0; r < MAP_SIZE; r++)
        for (int c = 0; c < MAP_SIZE; c++)
            map[r][c] = SYM_EMPTY;
    for (int i = 0; i < NUM_WALLS; i++) {
        int r = wallR[i], c = wallC[i];
        if (r >= 0 && r < MAP_SIZE && c >= 0 && c < MAP_SIZE)
            map[r][c] = SYM_WALL;
    }
    map[WH_ROW][WH_COL] = SYM_WAREHOUSE;
}

static void placeOrders(char map[][MAP_SIZE], order *headO) {
    for (order *o = headO; o != NULL; o = o->next) {
        int r = clampVal(o->x, 0, MAP_SIZE - 1);
        int c = clampVal(o->y, 0, MAP_SIZE - 1);
        if (map[r][c] == SYM_WAREHOUSE || map[r][c] == SYM_WALL) continue;
        if      (o->status == 0) map[r][c] = SYM_PENDING;
        else if (o->status == 1) map[r][c] = SYM_SHIPPING;
        else                     map[r][c] = SYM_DELIVERED;
    }
}

static void placeShippers(char map[][MAP_SIZE], shipper *headS) {
    for (shipper *s = headS; s != NULL; s = s->next) {
        if (s->x == 0 && s->y == 0) continue;
        int r = clampVal(s->x, 0, MAP_SIZE - 1);
        int c = clampVal(s->y, 0, MAP_SIZE - 1);
        if (map[r][c] == SYM_WAREHOUSE || map[r][c] == SYM_WALL) continue;
        map[r][c] = SYM_SHIPPER;
    }
}

// ================================================================
//  BFS TIM DUONG NGAN NHAT
// ================================================================
static int bfsFind(char baseMap[][MAP_SIZE],
                   int sr, int sc, int er, int ec,
                   MapPoint path[], int *pathLen) {
    bool     vis[MAP_SIZE][MAP_SIZE];
    MapPoint par[MAP_SIZE][MAP_SIZE];
    for (int r = 0; r < MAP_SIZE; r++)
        for (int c = 0; c < MAP_SIZE; c++) {
            vis[r][c] = false;
            par[r][c].x = -1; par[r][c].y = -1;
        }
    MapPoint q[MAP_SIZE * MAP_SIZE];
    int head = 0, tail = 0;
    q[tail].x = sr; q[tail].y = sc; tail++;
    vis[sr][sc] = true;

    while (head < tail) {
        MapPoint cur = q[head++];
        if (cur.x == er && cur.y == ec) {
            MapPoint rev[MAP_SIZE * MAP_SIZE];
            int len = 0;
            MapPoint at; at.x = er; at.y = ec;
            while (!(at.x == sr && at.y == sc)) {
                rev[len++] = at;
                at = par[at.x][at.y];
            }
            rev[len].x = sr; rev[len].y = sc; len++;
            *pathLen = 0;
            for (int i = len - 1; i >= 0; i--)
                path[(*pathLen)++] = rev[i];
            return 1;
        }
        for (int d = 0; d < 4; d++) {
            int nr = cur.x + dRow[d];
            int nc = cur.y + dCol[d];
            if (nr < 0 || nr >= MAP_SIZE || nc < 0 || nc >= MAP_SIZE) continue;
            if (vis[nr][nc] || baseMap[nr][nc] == SYM_WALL) continue;
            vis[nr][nc] = true;
            par[nr][nc] = cur;
            q[tail].x = nr; q[tail].y = nc; tail++;
        }
    }
    return 0;
}

// ================================================================
//  IN BAN DO HINH VUONG (2 ky tu moi o)
//  30 o * 2 = 60 ky tu rong  |  30 hang cao
//  -> Xap xi hinh vuong trong console (char cao ~ 2x rong)
// ================================================================
static void printMapSquare(char map[][MAP_SIZE]) {
    /* Vien tren */
    printf("  +");
    for (int c = 0; c < MAP_SIZE; c++) printf("--");
    printf("+\n");
    /* Noi dung */
    for (int r = 0; r < MAP_SIZE; r++) {
        printf("  |");
        for (int c = 0; c < MAP_SIZE; c++)
            printf("%c ", map[r][c]);
        printf("|\n");
    }
    /* Vien duoi */
    printf("  +");
    for (int c = 0; c < MAP_SIZE; c++) printf("--");
    printf("+\n");
}
/* Tong so dong printMapSquare xuat ra = MAP_SIZE + 2 = 32 */

/* Legend 1 dong (khong co dong trong o truoc, de caller tu quan ly) */
static void printLegendBFS(void) {
    printf("  W=Warehouse  S=Shipper  ==Path  G=Shipping  D=Delivered  X=Wall  .=Empty\n");
}

/* Header animation: luon in dung 4 dong de gotoXY ghi de sach */
static void printAnimHeader(int step, int total,
                            int curR, int curC, int destR, int destC,
                            const char *code, const char *cust,
                            const char *phase) {
    printf("  +----------------------------------------------------------------+\n");
    printf("  |  %-11s  Order:%-6s  Customer:%-22s|\n", phase, code, cust);
    printf("  |  Step:%3d/%-3d  Position:(%2d,%2d) -> Dest:(%2d,%2d)           |\n",
           step, total, curR, curC, destR, destC);
    printf("  +----------------------------------------------------------------+\n");
}
/* Header = 4 dong
   Blank  = 1 dong
   Map    = 32 dong
   Legend = 1 dong
   TONG   = 38 dong (rows 0..37) moi frame */

// ================================================================
//  TIM DON HANG GAN NHAT (BFS) TU VI TRI HIEN TAI
// ================================================================
static order* findNearest(order *headO, char baseMap[][MAP_SIZE],
                          int curR, int curC) {
    order   *nearest  = NULL;
    int      minDist  = 99999;
    char     tmp[MAP_SIZE][MAP_SIZE];

    for (order *o = headO; o != NULL; o = o->next) {
        if (o->status != 1) continue;
        int er = clampVal(o->x, 0, MAP_SIZE - 1);
        int ec = clampVal(o->y, 0, MAP_SIZE - 1);
        memcpy(tmp, baseMap, sizeof(char) * MAP_SIZE * MAP_SIZE);
        MapPoint path[MAP_SIZE * MAP_SIZE]; int pl = 0;
        if (bfsFind(tmp, curR, curC, er, ec, path, &pl)) {
            if (pl < minDist) { minDist = pl; nearest = o; }
        }
    }
    return nearest;
}

// ================================================================
//  CHUC NANG 1: DIEU PHOI DON HANG TU DONG
// ================================================================
void dispatchOrders(order **headO, shipper **headS) {
    system("cls");
    if (*headO == NULL || *headS == NULL) {
        printf("\n  [!] Missing orders or shippers!\n");
        printf("\n  Press any key to return...");
        _getch(); return;
    }

    time_t t = time(NULL);
    struct tm ti = *localtime(&t);
    char dateStr[20], timeStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &ti);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ti);

    printf("\n");
    printf("  ============================================================\n");
    printf("  |           AUTOMATIC ORDER DISPATCH SYSTEM                 |\n");
    printf("  |            Date: %-12s           Time: %-10s              |\n", dateStr, timeStr);
    printf("  ============================================================\n\n");

    int totalDispatched = 0;
    double totalWeight  = 0;

    for (shipper *s = *headS; s != NULL; s = s->next) {
        if (s->status != 0) continue;
        int hasOrder = 0;
        double curLoad = 0;
        for (order *o = *headO; o != NULL; o = o->next) {
            if (o->status == 0 && o->priority == s->prioritySP
                && (curLoad + o->weight) <= s->weight) {
                if (hasOrder == 0)
                    printf("  >> Shipper [%s] - %s (Max load: %.1f kg)\n",
                           s->code, s->Name, s->weight);
                o->status = 1;
                curLoad  += o->weight;
                hasOrder++; totalDispatched++;
                totalWeight += o->weight;
                printf("     [+] Order %-5s | %-18s | %.1f kg -> ASSIGNED\n",
                       o->code, o->customerName, o->weight);
            }
        }
        if (hasOrder > 0) {
            s->status = 1;
            printf("         Load: %.1f/%.1f kg\n\n", curLoad, s->weight);
        }
    }

    if (totalDispatched == 0) {
        printf("  [!] No orders were assigned.\n");
        printf("      (Check priority, capacity, and current statuses)\n");
    } else {
        printf("  ------------------------------------------------------------\n");
        printf("  Total assigned: %d orders  |  Total weight: %.1f kg\n",
               totalDispatched, totalWeight);
    }

    FILE *f = fopen("dispatch_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "|                       ORDER DISPATCH REPORT                                  |\n");
        fprintf(f, "|              Date: %-12s         Time: %-10s                                 |\n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  %-6s | %-20s | %-20s | %-8s | %-12s | %-10s |\n",
                "ORDER", "ITEM NAME", "CUSTOMER", "WEIGHT", "STATUS", "FEE (VND)");
        fprintf(f, "  -------+----------------------+----------------------+----------+--------------+------------\n");
        int cntOk = 0, cntFail = 0;
        for (order *o = *headO; o != NULL; o = o->next) {
            const char *st; double fee = 0;
            if      (o->status == 0) { st = "Pending";   cntFail++; }
            else if (o->status == 1) { st = "Shipping";  cntOk++; }
            else                     { st = "Delivered"; fee = o->fee; cntOk++; }
            fprintf(f, "  %-6.6s | %-20.20s | %-20.20s | %5.2f kg | %-12.12s | %10.2f\n",
                    o->code, o->orderName, o->customerName, o->weight, st, fee);
        }
        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  SUMMARY:\n");
        fprintf(f, "  - Processed (Shipping + Delivered) : %d orders\n", cntOk);
        fprintf(f, "  - Not assigned (Pending)           : %d orders\n", cntFail);
        fprintf(f, "  - Total assigned weight            : %.2f kg\n", totalWeight);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Saved to 'dispatch_report.txt'\n");
        system("start notepad dispatch_report.txt");
    }

    printf("\n  Press any key to return...");
    _getch();
}

// ================================================================
//  CHUC NANG 2: ANIMATION SHIPPER GIAO HANG
//  * Ban do 30x30 hinh vuong (2 ky tu/o)
//  * Khong nháy: dung gotoXY ghi de len man hinh
//  * Don dau: xuat phat tu kho W(0,0)
//  * Don tiep: tiep tuc tu vi tri vua giao, tim don GAN NHAT (BFS)
// ================================================================
void animateDelivery(order **headO, shipper **headS) {
    system("cls");
    enableAnsiControl();

    int hasShipping = 0;
    for (order *o = *headO; o != NULL; o = o->next)
        if (o->status == 1) { hasShipping = 1; break; }
    if (!hasShipping) {
        printf("\n  [!] No Shipping orders found!\n");
        printf("      Please run dispatch first (function [1]).\n");
        printf("\n  Press any key to return...");
        _getch(); return;
    }

    time_t t = time(NULL);
    struct tm ti = *localtime(&t);
    char dateStr[20], timeStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &ti);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ti);

    /* Ban do nen */
    char baseMap[MAP_SIZE][MAP_SIZE];
    initMap(baseMap);
    placeOrders(baseMap, *headO);

    /* Vi tri hien tai cua shipper - bat dau tu kho */
    int curR = WH_ROW, curC = WH_COL;

    /* Bao cao */
    int  reportCount = 0, totalDone = 0;
    char rCode[100][6]; char rCust[100][30];
    int  rSteps[100];   int  rOk[100];

    hideCursor();
    printf("\x1b[2J\x1b[H");

    /* Toa do frame animation co dinh tren terminal (0-based) */
    const int mapContentStartRow = 5; /* sau 3 dong header + 1 dong trong + vien tren map */
    const int mapContentStartCol = 3; /* "  |" truoc ky tu o dau tien */
    const int statusRow = mapContentStartRow + MAP_SIZE + 2;
    const int doneMsgRow = statusRow + 1;

    while (1) {
        /* Tim don Shipping gan nhat tu vi tri hien tai */
        order *o = findNearest(*headO, baseMap, curR, curC);
        if (o == NULL) break;  /* Het don */

        int er = clampVal(o->x, 0, MAP_SIZE - 1);
        int ec = clampVal(o->y, 0, MAP_SIZE - 1);

        strncpy(rCode[reportCount], o->code, 5);
        rCode[reportCount][5] = '\0';
        strncpy(rCust[reportCount], o->customerName, 29);
        rCust[reportCount][29] = '\0';

        MapPoint path[MAP_SIZE * MAP_SIZE]; int pathLen = 0;
        char tmpMap[MAP_SIZE][MAP_SIZE];
        memcpy(tmpMap, baseMap, sizeof(baseMap));

        if (!bfsFind(tmpMap, curR, curC, er, ec, path, &pathLen)) {
            /* Khong tim duoc duong - bo qua don nay */
            rOk[reportCount] = 0; rSteps[reportCount] = 0; reportCount++;
            o->status = 2;
            continue;
        }

        /* Ve frame 1 lan cho moi don, sau do chi cap nhat o da thay doi */
        {
            char animMap[MAP_SIZE][MAP_SIZE];
            memcpy(animMap, baseMap, sizeof(baseMap));
            if (pathLen > 0) {
                int sr0 = path[0].x, sc0 = path[0].y;
                if (animMap[sr0][sc0] != SYM_WAREHOUSE) animMap[sr0][sc0] = SYM_SHIPPER;
            }

            gotoXY(0, 0);
            printf("  +----------------------------------------------------------------+\n");
            printf("  |  DELIVERING ORDER: %-6.6s | CUSTOMER: %-24.24s |\n", o->code, o->customerName);
            printf("  +----------------------------------------------------------------+\n\n");
            printMapSquare(animMap);
            printLegendBFS();
            printf("  Status: Route simulation running (cell updates only).            \n");
            fflush(stdout);
        }

        for (int step = 1; step < pathLen; step++) {
            int pr = path[step - 1].x, pc = path[step - 1].y;
            int cr = path[step].x, cc = path[step].y;

            /* O vua di qua: luu vet duong '=' tren moi o di qua (tru kho/vat can) */
            if (baseMap[pr][pc] != SYM_WAREHOUSE && baseMap[pr][pc] != SYM_WALL) {
                baseMap[pr][pc] = SYM_PATH;
                gotoXY(mapContentStartRow + pr, mapContentStartCol + pc * 2);
                printf("%c", SYM_PATH);
            }

            /* O hien tai cua shipper */
            if (baseMap[cr][cc] != SYM_WAREHOUSE) {
                gotoXY(mapContentStartRow + cr, mapContentStartCol + cc * 2);
                printf("%c", SYM_SHIPPER);
            }

            gotoXY(statusRow, 2);
            printf("  Status: Order %-6.6s moving (%2d/%-2d).                        ",
                   o->code, step, pathLen - 1);
            fflush(stdout);
            Sleep(500);
        }

        /* Giao xong */
        o->status = 2;
        baseMap[er][ec] = SYM_DELIVERED;
        gotoXY(mapContentStartRow + er, mapContentStartCol + ec * 2);
        printf("%c", SYM_DELIVERED);
        gotoXY(doneMsgRow, 2);
        printf("  [DONE] Delivered order %-6.6s to customer %-25.25s",
               o->code, o->customerName);
        fflush(stdout);
        gotoXY(statusRow, 2);
        printf("  Status: Finding next order...                                  ");
        gotoXY(doneMsgRow + 1, 2);
        printf("                                                                  ");
        fflush(stdout);
        Sleep(500);
        totalDone++;
        curR = er; curC = ec;   /* Cap nhat vi tri */

        rOk[reportCount] = 1;
        rSteps[reportCount] = pathLen - 1;
        reportCount++;
    }
    showCursor();

    /* ===== KET QUA CUOI ===== */
    system("cls");
    char finalMap[MAP_SIZE][MAP_SIZE];
    initMap(finalMap);
    placeOrders(finalMap, *headO);

    printf("\n");
    printf("  +----------------------------------------------------------------+\n");
    printf("  |             DELIVERY COMPLETED - FINAL RESULT                  |\n");
    printf("  |          Date: %-12s               Time: %-10s                 |\n", dateStr, timeStr);
    printf("  +----------------------------------------------------------------+\n\n");
    printMapSquare(finalMap);
    printLegendBFS();

    printf("\n");
    printf("  +------+----------+------------------+------------+\n");
    printf("  |  NO. | ORDER ID | RESULT           | STEPS      |\n");
    printf("  +------+----------+------------------+------------+\n");
    for (int i = 0; i < reportCount; i++) {
        printf("  | %-4d | %-8s | %-16s | %-9d |\n",
               i + 1, rCode[i],
               rOk[i] ? "DELIVERED" : "NO ROUTE",
               rSteps[i]);
    }
    printf("  +------+----------+------------------+------------+\n");
    printf("  +-------------------------------------------------+\n");
    printf("  |  Total successful deliveries: %-3d orders       |\n", totalDone);
    printf("  +-------------------------------------------------+\n");

    /* Ghi file */
    FILE *f = fopen("animation_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "|                      DELIVERY SIMULATION REPORT                              |\n");
        fprintf(f, "|                    Date: %-12s         Time: %-10s                           |\n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  %-4.4s | %-6.6s | %-20.20s | %-20.20s | %-16.16s | %-9.9s\n",
                "NO.", "ORDER", "ITEM NAME", "CUSTOMER", "RESULT", "STEPS");
        fprintf(f, "  -----+--------+----------------------+----------------------+------------------+-----------\n");
        int idx = 0;
        for (order *o2 = *headO; o2 != NULL && idx < reportCount; o2 = o2->next) {
            if (o2->status != 2) continue;
            fprintf(f, "  %-4d | %-6.6s | %-20.20s | %-20.20s | %-16.16s | %-9d\n",
                    idx + 1, rCode[idx], o2->orderName, rCust[idx],
                    rOk[idx] ? "DELIVERED" : "NO ROUTE",
                    rSteps[idx]);
            idx++;
        }
        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  SUMMARY:\n");
        fprintf(f, "  - Total delivered orders : %d\n", totalDone);
        fprintf(f, "  - Total no-route orders  : %d\n", reportCount - totalDone);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Saved to 'animation_report.txt'\n");
        system("start notepad animation_report.txt");
    }

    printf("\n  Press any key to return...");
    _getch();
}

// ================================================================
//  CHUC NANG 3: TONG QUAN KHO HANG
//  So do mat bang: 9 khu, moi khu = khoi 3x3 ky hieu ###
//  Sap xep 3 hang x 3 cot, giua cac khu cach 1 o trong
// ================================================================
void warehouseOverview(order **headO, shipper **headS) {
    system("cls");

    int cntP = 0, cntG = 0, cntD = 0;
    for (order *o = *headO; o != NULL; o = o->next) {
        if      (o->status == 0) cntP++;
        else if (o->status == 1) cntG++;
        else                     cntD++;
    }
    int totalO = cntP + cntG + cntD;

    int cntFree = 0, cntBusy = 0;
    for (shipper *s = *headS; s != NULL; s = s->next) {
        if (s->status == 0) cntFree++;
        else                cntBusy++;
    }
    int totalS = cntFree + cntBusy;

    time_t t = time(NULL);
    struct tm ti = *localtime(&t);
    char dateStr[20], timeStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &ti);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ti);

    /* Thong ke tong quat */
    printf("\n");
    printf("  ============================================================\n");
    printf("  |            WAREHOUSE OVERVIEW - SMART DELIVERY           |\n");
    printf("  |           Date: %-12s           Time: %-10s              |\n", dateStr, timeStr);
    printf("  ============================================================\n");
    printf("  Orders   | Total:%-3d  P(wait):%-3d  G(ship):%-3d  D(done):%-3d\n",
           totalO, cntP, cntG, cntD);
    printf("  Shipper  | Total:%-3d  Free: %-3d  Busy: %-3d\n\n",
           totalS, cntFree, cntBusy);

    if (totalO == 0)
        printf("  >> Warehouse is empty, no orders yet.\n");
    else if (cntP == 0)
        printf("  >> All orders have been processed!\n");
    else if (cntP > 5)
        printf("  >> [WARNING] Warehouse overloaded! %d orders are waiting.\n", cntP);
    else
        printf("  >> Stable. %d orders are waiting for delivery.\n", cntP);
    if (cntFree == 0 && cntP > 0)
        printf("  >> [WARNING] No free shipper available!\n");

    /* ----------------------------------------------------------------
       SO DO MAT BANG: 9 khu (3 hang x 3 cot)
       Moi khu = 3 hang ###, ten khu o tren, cach khu ben phai = 3 dau cach
       Cach khu ben duoi = 1 dong trong
    ---------------------------------------------------------------- */
    printf("\n");
    printf("  ============================================================\n");
    printf("  |                  WAREHOUSE LAYOUT MAP                    |\n");
    printf("  ============================================================\n\n");

    /* Ten cac khu (3x3 = 9 khu) */
    const char *tenKhu[3][3] = {
        {"[KHU A1]", "[KHU A2]", "[KHU A3]"},
        {"[KHU B1]", "[KHU B2]", "[KHU B3]"},
        {"[KHU C1]", "[KHU C2]", "[KHU C3]"}
    };

    printf("  +-----------------------------------------------------------+\n");
    printf("  |                   WAREHOUSE LAYOUT FRAME                 |\n");
    printf("  +-----------------------------------------------------------+\n");

    /*
       Moi khu duoc ve thanh 1 o co vien:
       +---------+
       | [KHU A1]|
       |   ###   |
       |   ###   |
       |   ###   |
       +---------+
    */
    for (int bi = 0; bi < 3; bi++) {
        printf("  |  +---------+    +---------+    +---------+  |\n");
        printf("  |  | %-8s|    | %-8s|    | %-8s|  |\n",
               tenKhu[bi][0], tenKhu[bi][1], tenKhu[bi][2]);
        for (int row = 0; row < 3; row++) {
            printf("  |  |   ###   |    |   ###   |    |   ###   |  |\n");
        }
        printf("  |  +---------+    +---------+    +---------+  |\n");
        printf("\n");
    }
    printf("  +-----------------------------------------------------------+\n");

    /* Chu thich */
    printf("  -----------------------------------------------------------\n");
    printf("  Notes:\n");
    printf("    ### = Storage area (each block is one warehouse zone)\n");
    printf("    [W] = Main warehouse start point at coordinate (0, 0)\n");
    printf("    Total: 9 zones | A1-A3 top row | B1-B3 middle row | C1-C3 bottom row\n");
    printf("  -----------------------------------------------------------\n");

    /* Ghi file */
    FILE *f = fopen("warehouse_overview.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "  |                      WAREHOUSE OVERVIEW REPORT                             |\n");
        fprintf(f, "  |            Date: %-12s         Time: %-10s                                 |\n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  [ORDERS]    Total: %-3d |  Pending(P): %-3d  |  Shipping(G): %-3d  |  Delivered(D): %-3d\n",
                totalO, cntP, cntG, cntD);
        fprintf(f, "  [SHIPPERS]  Total: %-3d |  Free: %-3d        |  Busy: %-3d\n\n",
                totalS, cntFree, cntBusy);

        if (totalO == 0) fprintf(f, "  >> Warehouse is empty.\n");
        else if (cntP == 0) fprintf(f, "  >> All orders processed!\n");
        else if (cntP > 5) fprintf(f, "  >> [WARNING] Warehouse overloaded! %d pending orders.\n", cntP);
        else fprintf(f, "  >> Stable. %d orders waiting.\n", cntP);
        if (cntFree == 0 && cntP > 0) fprintf(f, "  >> [WARNING] No free shipper available!\n");

        fprintf(f, "\n--------------------------------------------------------------------------------\n");
        fprintf(f, "  ORDER LIST:\n");
        fprintf(f, "--------------------------------------------------------------------------------\n");
        fprintf(f, "  %-6s | %-20s | %-20s | %-9s | %-10s | %-8s |\n",
                "ORDER", "ITEM NAME", "CUSTOMER", "STATUS", "FEE (VND)", "COORD");
        fprintf(f, "  -------+----------------------+----------------------+-----------+------------+----------\n");
        for (order *o = *headO; o != NULL; o = o->next) {
            const char *st = (o->status==0)?"Pending":(o->status==1)?"Shipping":"Delivered";
            double fee = (o->status == 2) ? o->fee : 0.0;
            fprintf(f, "  %-6.6s | %-20.20s | %-20.20s | %-9.9s | %10.2f | (%2d,%2d) |\n",
                    o->code, o->orderName, o->customerName, st, fee, o->x, o->y);
        }
        if (totalO == 0) fprintf(f, "  (No orders yet)\n");

        fprintf(f, "\n--------------------------------------------------------------------------------\n");
        fprintf(f, "  SHIPPER LIST:\n");
        fprintf(f, "--------------------------------------------------------------------------------\n");
        fprintf(f, "  %-6s | %-20s | %-14s | %-11s | %-10s | %-8s |\n",
                "SHIP ID", "SHIPPER NAME", "CCCD", "TYPE", "STATUS", "LOAD(KG)");
        fprintf(f, "  -------+----------------------+----------------+-------------+------------+----------\n");
        for (shipper *s = *headS; s != NULL; s = s->next) {
            const char *type = (s->prioritySP==1) ? "Express" : "Normal";
            const char *st   = (s->status==0)     ? "Free"    : "Busy";
            fprintf(f, "  %-6.6s | %-20.20s | %-14lld | %-11.11s | %-10.10s | %.2f |\n",
                    s->code, s->Name, s->CCCD, type, st, s->weight);
        }
        if (totalS == 0) fprintf(f, "  (No shippers yet)\n");

        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  NOTE: P = Pending | G = Shipping | D = Delivered\n");
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Saved to 'warehouse_overview.txt'\n");
        system("start notepad warehouse_overview.txt");
    }

    printf("\n  Press any key to return...");
    _getch();
}

// ================================================================
//  CHUC NANG 4: GOI Y DUONG DI TOI UU (ban do BFS, map vuong)
// ================================================================
void suggestOptimalRoute(order **headO, shipper **headS) {
    system("cls");

    char baseMap[MAP_SIZE][MAP_SIZE];
    initMap(baseMap);
    placeOrders(baseMap, *headO);
    placeShippers(baseMap, *headS);

    time_t t = time(NULL);
    struct tm ti = *localtime(&t);
    char dateStr[20], timeStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &ti);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ti);

    printf("\n");
    printf("  ============================================================\n");
    printf("  |              OPTIMAL ROUTE SUGGESTION - BFS              |\n");
    printf("  |           Date: %-12s           Time: %-10s              |\n", dateStr, timeStr);
    printf("  ============================================================\n");
    printf("  Start warehouse: W(%d,%d) | Algorithm: BFS\n\n", WH_ROW, WH_COL);

    printf("  [OVERVIEW MAP]\n");
    printMapSquare(baseMap);
    printLegendBFS();

    int orderNum = 0, foundPath = 0;

    for (order *o = *headO; o != NULL; o = o->next) {
        if (o->status != 0) continue;
        orderNum++;

        int er = clampVal(o->x, 0, MAP_SIZE - 1);
        int ec = clampVal(o->y, 0, MAP_SIZE - 1);

        printf("\n  ----------------------------------------------------------\n");
        printf("  Order %d: %-6s | Customer: %-20s | Coordinate:(%d,%d)\n",
               orderNum, o->code, o->customerName, er, ec);
        printf("  ----------------------------------------------------------\n");

        char routeMap[MAP_SIZE][MAP_SIZE];
        memcpy(routeMap, baseMap, sizeof(baseMap));
        MapPoint path[MAP_SIZE * MAP_SIZE]; int pathLen = 0;

        if (bfsFind(routeMap, WH_ROW, WH_COL, er, ec, path, &pathLen)) {
            foundPath++;
            for (int i = 1; i < pathLen - 1; i++) {
                int r = path[i].x, c = path[i].y;
                if (routeMap[r][c] == SYM_EMPTY) routeMap[r][c] = SYM_PATH;
            }
            printMapSquare(routeMap);
            printf("  Distance: %d steps  |  Priority: %s  |  Weight: %.1f kg\n",
                   pathLen - 1,
                   (o->priority == 1) ? "EXPRESS" : "NORMAL",
                   o->weight);
        } else {
            printf("  [!] NO PATH FOUND (blocked by obstacles)\n");
        }
    }

    if (orderNum == 0)
        printf("\n  [!] No Pending orders to suggest!\n");
    else {
        printf("\n  ============================================================\n");
        printf("  Total Pending: %d  |  Found: %d  |  Not found: %d\n",
               orderNum, foundPath, orderNum - foundPath);
        printf("  ============================================================\n");
    }

    /* Ghi file */
    FILE *f = fopen("route_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "|                     OPTIMAL ROUTE SUGGESTION REPORT                          |\n");
        fprintf(f, "|              Date: %-12s         Time: %-10s                                 |\n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  Start warehouse: W(%d, %d)  |  Algorithm: BFS (shortest path)\n\n",
                WH_ROW, WH_COL);
        fprintf(f, "  %-4s | %-6s | %-20s | %-20s | %-9s | %-8s | %-14s |\n",
                "NO.", "ORDER", "ITEM NAME", "CUSTOMER", "COORD", "STEPS", "RESULT");
        fprintf(f, "  -----+--------+----------------------+----------------------+-----------+----------+----------------\n");

        char tmpBase[MAP_SIZE][MAP_SIZE];
        initMap(tmpBase); placeOrders(tmpBase, *headO);

        int num2 = 0, found2 = 0;
        for (order *o = *headO; o != NULL; o = o->next) {
            if (o->status != 0) continue;
            num2++;
            int er = clampVal(o->x, 0, MAP_SIZE - 1);
            int ec = clampVal(o->y, 0, MAP_SIZE - 1);
            char tm2[MAP_SIZE][MAP_SIZE];
            memcpy(tm2, tmpBase, sizeof(tmpBase));
            MapPoint path[MAP_SIZE * MAP_SIZE]; int pl = 0;
            int ok = bfsFind(tm2, WH_ROW, WH_COL, er, ec, path, &pl);
            if (ok) found2++;
            fprintf(f, "  %-4d | %-6.6s | %-20.20s | %-20.20s | (%2d,%2d)   | %-8d | %-14.14s |\n",
                    num2, o->code, o->orderName, o->customerName,
                    er, ec, ok ? pl - 1 : 0,
                    ok ? "FOUND" : "NOT FOUND");
        }
        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  SUMMARY:\n");
        fprintf(f, "  - Total Pending orders : %d\n", num2);
        fprintf(f, "  - Found valid routes   : %d\n", found2);
        fprintf(f, "  - Routes not found     : %d\n", num2 - found2);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Saved to 'route_report.txt'\n");
        system("start notepad route_report.txt");
    }

    printf("\n  Press any key to return...");
    _getch();
}

// ================================================================
//  OPTION 3 - MENU CHINH
// ================================================================
int Smart_Coordination(order **headO, shipper **headS) {
    int countWrong = 0;
    int choice;

    while (1) {
        system("cls");
        printf("\n");
        printf("  ================================================\n");
        printf("  |       3. SMART COORDINATION MENU             |\n");
        printf("  ================================================\n");
        printf("  | [1]. Automatic order dispatch                |\n");
        printf("  | [2]. Delivery simulation (animation)         |\n");
        printf("  | [3]. Warehouse overview (layout map)         |\n");
        printf("  | [4]. Optimal route suggestion (BFS)          |\n");
        printf("  | [5]. Back to main menu                       |\n");
        printf("  ================================================\n\n");

        if (countWrong >= 3) {
            printf("  [!] Wrong input 3 times. Feature locked!\n");
            printf("  Press any key to return...");
            _getch();
            return -1;
        }

        printf("  Enter your choice (1-5): ");
        fflush(stdin);
        scanf("%d", &choice);

        switch (choice) {
            case 1: dispatchOrders(headO, headS);      countWrong = 0; break;
            case 2: animateDelivery(headO, headS);     countWrong = 0; break;
            case 3: warehouseOverview(headO, headS);   countWrong = 0; break;
            case 4: suggestOptimalRoute(headO, headS); countWrong = 0; break;
            case 5: return 0;
            default:
                printf("\n  [!] Please enter a number from 1 to 5.\n");
                countWrong++;
                Sleep(800);
                break;
        }
    }
    return 0;
}
