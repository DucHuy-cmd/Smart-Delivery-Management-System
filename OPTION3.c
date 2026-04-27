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
    printf("  W=Kho  S=Shipper  ==Duong  G=Don giao  D=Da giao  X=Vat can  .=Trong\n");
}

/* Header animation: luon in dung 4 dong de gotoXY ghi de sach */
static void printAnimHeader(int step, int total,
                            int curR, int curC, int destR, int destC,
                            const char *code, const char *cust,
                            const char *phase) {
    printf("  +----------------------------------------------------------------+\n");
    printf("  |  %-11s  Don:%-6s  KH:%-28s|\n", phase, code, cust);
    printf("  |  Buoc:%3d/%-3d  Vi tri:(%2d,%2d)  -> Dich:(%2d,%2d)            |\n",
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
        printf("\n  [!] Thieu don hang hoac shipper!\n");
        printf("\n  Nhan phim bat ky de quay lai...");
        _getch(); return;
    }

    time_t t = time(NULL);
    struct tm ti = *localtime(&t);
    char dateStr[20], timeStr[20];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &ti);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ti);

    printf("\n");
    printf("  ============================================================\n");
    printf("          HE THONG DIEU PHOI DON HANG TU DONG              \n");
    printf("    Ngay: %-12s           Gio: %-10s            \n", dateStr, timeStr);
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
                    printf("  >> Shipper [%s] - %s (Tai max: %.1f kg)\n",
                           s->code, s->Name, s->weight);
                o->status = 1;
                curLoad  += o->weight;
                hasOrder++; totalDispatched++;
                totalWeight += o->weight;
                printf("     [+] Don %-5s | %-18s | %.1f kg -> PHAN CONG\n",
                       o->code, o->customerName, o->weight);
            }
        }
        if (hasOrder > 0) {
            s->status = 1;
            printf("         Tai: %.1f/%.1f kg\n\n", curLoad, s->weight);
        }
    }

    if (totalDispatched == 0) {
        printf("  [!] Khong co don nao duoc phan cong.\n");
        printf("      (Kiem tra: do uu tien, tai trong, trang thai don/shipper)\n");
    } else {
        printf("  ------------------------------------------------------------\n");
        printf("  Tong da phan cong: %d don  |  Tong can nang: %.1f kg\n",
               totalDispatched, totalWeight);
    }

    FILE *f = fopen("dispatch_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "                      BAO CAO DIEU PHOI DON HANG                              \n");
        fprintf(f, "              Ngay: %-12s          Gio: %-10s               \n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  %-6s | %-20s | %-20s | %-8s | %-12s | %-10s\n",
                "MA DON", "TEN HANG", "KHACH HANG", "CAN NANG", "TRANG THAI", "PHI (VND)");
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
        fprintf(f, "  TONG KET:\n");
        fprintf(f, "  - Da duoc xu ly (Shipping + Delivered) : %d don\n", cntOk);
        fprintf(f, "  - Chua duoc phan cong    (Pending)     : %d don\n", cntFail);
        fprintf(f, "  - Tong can nang da phan cong           : %.2f kg\n", totalWeight);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Da luu -> 'dispatch_report.txt'\n");
        system("start notepad dispatch_report.txt");
    }

    printf("\n  Nhan phim bat ky de quay lai...");
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
        printf("\n  [!] Khong co don Shipping nao!\n");
        printf("      Hay dieu phoi don hang truoc (chuc nang [1]).\n");
        printf("\n  Nhan phim bat ky de quay lai...");
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
    int doneMsgRow = statusRow + 1;

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
            printf("  |  DANG GIAO DON: %-6.6s  | KH: %-29.29s |\n", o->code, o->customerName);
            printf("  +----------------------------------------------------------------+\n\n");
            printMapSquare(animMap);
            printLegendBFS();
            printf("  Trang thai: Dang mo phong duong di (cap nhat o, khong redraw map). \n");
            fflush(stdout);
        }

        for (int step = 1; step < pathLen; step++) {
            int pr = path[step - 1].x, pc = path[step - 1].y;
            int cr = path[step].x, cc = path[step].y;

            /* O vua di qua: luu vet duong '=' neu o trong */
            if (baseMap[pr][pc] == SYM_EMPTY) {
                baseMap[pr][pc] = SYM_PATH;
                gotoXY(mapContentStartRow + pr, mapContentStartCol + pc * 2);
                printf("%c", SYM_PATH);
            } else if (baseMap[pr][pc] != SYM_WAREHOUSE) {
                gotoXY(mapContentStartRow + pr, mapContentStartCol + pc * 2);
                printf("%c", baseMap[pr][pc]);
            }

            /* O hien tai cua shipper */
            if (baseMap[cr][cc] != SYM_WAREHOUSE) {
                gotoXY(mapContentStartRow + cr, mapContentStartCol + cc * 2);
                printf("%c", SYM_SHIPPER);
            }

            gotoXY(statusRow, 2);
            printf("  Trang thai: Don %-6.6s dang di (%2d/%-2d).                     ",
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
        printf("  [DONE] Da giao xong don %-6.6s cho KH %-29.29s",
               o->code, o->customerName);
        fflush(stdout);
        doneMsgRow++;
        gotoXY(statusRow, 2);
        printf("  Trang thai: Dang tim don tiep theo...                          ");
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
    printf("  |           HOAN THANH CHUYEN GIAO - KET QUA CUOI              |\n");
    printf("  |  Ngay: %-12s               Gio: %-10s              |\n", dateStr, timeStr);
    printf("  +----------------------------------------------------------------+\n\n");
    printMapSquare(finalMap);
    printLegendBFS();

    printf("\n");
    printf("  +------+----------+------------------+------------+\n");
    printf("  |  STT | MA DON   | KET QUA          | SO BUOC    |\n");
    printf("  +------+----------+------------------+------------+\n");
    for (int i = 0; i < reportCount; i++) {
        printf("  | %-4d | %-8s | %-16s | %-9d |\n",
               i + 1, rCode[i],
               rOk[i] ? "GIAO THANH CONG" : "KHONG TIM DUOC",
               rSteps[i]);
    }
    printf("  +------+----------+------------------+------------+\n");
    printf("  +-------------------------------------------------+\n");
    printf("  |  Tong giao thanh cong: %-3d don                 |\n", totalDone);
    printf("  +-------------------------------------------------+\n");

    /* Ghi file */
    FILE *f = fopen("animation_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "                    BAO CAO MO PHONG GIAO HANG                                \n");
        fprintf(f, "              Ngay: %-12s          Gio: %-10s               \n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  %-4.4s | %-6.6s | %-20.20s | %-20.20s | %-16.16s | %-9.9s\n",
                "STT", "MA DON", "TEN HANG", "KHACH HANG", "KET QUA", "SO BUOC");
        fprintf(f, "  -----+--------+----------------------+----------------------+------------------+-----------\n");
        int idx = 0;
        for (order *o2 = *headO; o2 != NULL && idx < reportCount; o2 = o2->next) {
            if (o2->status != 2) continue;
            fprintf(f, "  %-4d | %-6.6s | %-20.20s | %-20.20s | %-16.16s | %-9d\n",
                    idx + 1, rCode[idx], o2->orderName, rCust[idx],
                    rOk[idx] ? "GIAO THANH CONG" : "KHONG TIM DUOC",
                    rSteps[idx]);
            idx++;
        }
        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  TONG KET:\n");
        fprintf(f, "  - Tong don giao thanh cong  : %d don\n", totalDone);
        fprintf(f, "  - Tong don khong tim duoc   : %d don\n", reportCount - totalDone);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Da luu -> 'animation_report.txt'\n");
        system("start notepad animation_report.txt");
    }

    printf("\n  Nhan phim bat ky de quay lai...");
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
    printf("           TONG QUAN KHO HANG - SMART DELIVERY              \n");
    printf("          Ngay: %-12s           Gio: %-10s            \n", dateStr, timeStr);
    printf("  ============================================================\n");
    printf("  Don hang | Tong:%-3d  P(cho):%-3d  G(dang):%-3d  D(xong):%-3d\n",
           totalO, cntP, cntG, cntD);
    printf("  Shipper  | Tong:%-3d  Ranh: %-3d  Dang ban: %-3d\n\n",
           totalS, cntFree, cntBusy);

    if (totalO == 0)
        printf("  >> Kho trong, chua co don hang.\n");
    else if (cntP == 0)
        printf("  >> Tat ca don hang da duoc xu ly!\n");
    else if (cntP > 5)
        printf("  >> [CANH BAO] Kho qua tai! Con %d don dang cho.\n", cntP);
    else
        printf("  >> On dinh. Con %d don cho giao.\n", cntP);
    if (cntFree == 0 && cntP > 0)
        printf("  >> [CANH BAO] Khong con shipper ranh!\n");

    /* ----------------------------------------------------------------
       SO DO MAT BANG: 9 khu (3 hang x 3 cot)
       Moi khu = 3 hang ###, ten khu o tren, cach khu ben phai = 3 dau cach
       Cach khu ben duoi = 1 dong trong
    ---------------------------------------------------------------- */
    printf("\n");
    printf("  ============================================================\n");
    printf("                   SO DO MAT BANG KHO HANG                  \n");
    printf("  ============================================================\n\n");

    /* Ten cac khu (3x3 = 9 khu) */
    const char *tenKhu[3][3] = {
        {"[KHU A1]", "[KHU A2]", "[KHU A3]"},
        {"[KHU B1]", "[KHU B2]", "[KHU B3]"},
        {"[KHU C1]", "[KHU C2]", "[KHU C3]"}
    };

    printf("  +-----------------------------------------------------------+\n");
    printf("  |                    KHUNG MO PHONG KHO                    |\n");
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
    printf("  Chu thich:\n");
    printf("    ### = Khu luu tru hang hoa (moi khu = 1 vung mat bang)\n");
    printf("    [W] = Kho xuat phat chinh tai toa do (0, 0) tren ban do\n");
    printf("    Tong: 9 khu  |  A1-A3: Hang tren  |  B1-B3: Hang giua  |  C1-C3: Hang duoi\n");
    printf("  -----------------------------------------------------------\n");

    /* Ghi file */
    FILE *f = fopen("warehouse_overview.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "                    BAO CAO TONG QUAN KHO HANG                                \n");
        fprintf(f, "              Ngay: %-12s          Gio: %-10s               \n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  [DON HANG]  Tong: %-3d  |  Pending(P): %-3d  |  Shipping(G): %-3d  |  Delivered(D): %-3d\n",
                totalO, cntP, cntG, cntD);
        fprintf(f, "  [SHIPPER]   Tong: %-3d  |  Ranh: %-3d        |  Dang ban: %-3d\n\n",
                totalS, cntFree, cntBusy);

        if (totalO == 0) fprintf(f, "  >> Kho trong.\n");
        else if (cntP == 0) fprintf(f, "  >> Tat ca don da xu ly!\n");
        else if (cntP > 5) fprintf(f, "  >> [CANH BAO] Kho qua tai! Con %d don Pending.\n", cntP);
        else fprintf(f, "  >> On dinh. Con %d don cho giao.\n", cntP);
        if (cntFree == 0 && cntP > 0) fprintf(f, "  >> [CANH BAO] Khong con shipper ranh!\n");

        fprintf(f, "\n--------------------------------------------------------------------------------\n");
        fprintf(f, "  DANH SACH DON HANG:\n");
        fprintf(f, "--------------------------------------------------------------------------------\n");
        fprintf(f, "  %-6s | %-20s | %-20s | %-9s | %-10s | %-8s\n",
                "MA DON", "TEN HANG", "KHACH HANG", "TRANG THAI", "PHI (VND)", "TOA DO");
        fprintf(f, "  -------+----------------------+----------------------+-----------+------------+----------\n");
        for (order *o = *headO; o != NULL; o = o->next) {
            const char *st = (o->status==0)?"Pending":(o->status==1)?"Shipping":"Delivered";
            double fee = (o->status == 2) ? o->fee : 0.0;
            fprintf(f, "  %-6.6s | %-20.20s | %-20.20s | %-9.9s | %10.2f | (%2d,%2d)\n",
                    o->code, o->orderName, o->customerName, st, fee, o->x, o->y);
        }
        if (totalO == 0) fprintf(f, "  (Chua co don hang nao)\n");

        fprintf(f, "\n--------------------------------------------------------------------------------\n");
        fprintf(f, "  DANH SACH SHIPPER:\n");
        fprintf(f, "--------------------------------------------------------------------------------\n");
        fprintf(f, "  %-6s | %-20s | %-14s | %-11s | %-10s | %-8s\n",
                "MA SP", "TEN SHIPPER", "CCCD", "LOAI", "TRANG THAI", "TAI(KG)");
        fprintf(f, "  -------+----------------------+----------------+-------------+------------+----------\n");
        for (shipper *s = *headS; s != NULL; s = s->next) {
            const char *type = (s->prioritySP==1) ? "Hoa toc" : "Binh thuong";
            const char *st   = (s->status==0)     ? "Ranh"    : "Dang ban";
            fprintf(f, "  %-6.6s | %-20.20s | %-14lld | %-11.11s | %-10.10s | %.2f\n",
                    s->code, s->Name, s->CCCD, type, st, s->weight);
        }
        if (totalS == 0) fprintf(f, "  (Chua co shipper nao)\n");

        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  GHI CHU:  P = Pending (cho giao)  |  G = Shipping (dang giao)  |  D = Delivered\n");
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Da luu -> 'warehouse_overview.txt'\n");
        system("start notepad warehouse_overview.txt");
    }

    printf("\n  Nhan phim bat ky de quay lai...");
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
    printf("              GOI Y DUONG DI TOI UU - BFS                   \n");
    printf("    Ngay: %-12s           Gio: %-10s            \n", dateStr, timeStr);
    printf("  ============================================================\n");
    printf("  Kho xuat phat: W(%d,%d) | Thuat toan: BFS\n\n", WH_ROW, WH_COL);

    printf("  [BAN DO TONG QUAN]\n");
    printMapSquare(baseMap);
    printLegendBFS();

    int orderNum = 0, foundPath = 0;

    for (order *o = *headO; o != NULL; o = o->next) {
        if (o->status != 0) continue;
        orderNum++;

        int er = clampVal(o->x, 0, MAP_SIZE - 1);
        int ec = clampVal(o->y, 0, MAP_SIZE - 1);

        printf("\n  ----------------------------------------------------------\n");
        printf("  Don %d: %-6s | KH: %-20s | Toa do:(%d,%d)\n",
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
            printf("  Khoang cach: %d buoc  |  Uu tien: %s  |  Can nang: %.1f kg\n",
                   pathLen - 1,
                   (o->priority == 1) ? "HOA TOC" : "BINH THUONG",
                   o->weight);
        } else {
            printf("  [!] KHONG TIM DUOC DUONG DI (bi chan boi chuong ngai vat)\n");
        }
    }

    if (orderNum == 0)
        printf("\n  [!] Khong co don Pending nao de goi y!\n");
    else {
        printf("\n  ============================================================\n");
        printf("  Tong Pending: %d  |  Tim duoc: %d  |  Khong tim duoc: %d\n",
               orderNum, foundPath, orderNum - foundPath);
        printf("  ============================================================\n");
    }

    /* Ghi file */
    FILE *f = fopen("route_report.txt", "w");
    if (f) {
        fprintf(f, "================================================================================\n");
        fprintf(f, "                     BAO CAO GOI Y DUONG DI TOI UU                            \n");
        fprintf(f, "              Ngay: %-12s          Gio: %-10s               \n", dateStr, timeStr);
        fprintf(f, "================================================================================\n\n");
        fprintf(f, "  Kho xuat phat: W(%d, %d)  |  Thuat toan: BFS (tim duong ngan nhat)\n\n",
                WH_ROW, WH_COL);
        fprintf(f, "  %-4s | %-6s | %-20s | %-20s | %-9s | %-8s | %-14s\n",
                "STT", "MA DON", "TEN HANG", "KHACH HANG", "TOA DO", "SO BUOC", "KET QUA");
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
            fprintf(f, "  %-4d | %-6.6s | %-20.20s | %-20.20s | (%2d,%2d)   | %-8d | %-14.14s\n",
                    num2, o->code, o->orderName, o->customerName,
                    er, ec, ok ? pl - 1 : 0,
                    ok ? "TIM DUOC" : "KHONG TIM DUOC");
        }
        fprintf(f, "\n================================================================================\n");
        fprintf(f, "  TONG KET:\n");
        fprintf(f, "  - Tong don Pending      : %d don\n", num2);
        fprintf(f, "  - Tim duoc duong di     : %d don\n", found2);
        fprintf(f, "  - Khong tim duoc duong  : %d don\n", num2 - found2);
        fprintf(f, "================================================================================\n");
        fclose(f);
        printf("\n  [OK] Da luu -> 'route_report.txt'\n");
        system("start notepad route_report.txt");
    }

    printf("\n  Nhan phim bat ky de quay lai...");
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
        printf("  =       3. SMART COORDINATION MENU            =\n");
        printf("  ================================================\n");
        printf("  = [1]. Tu dong dieu phoi don hang             =\n");
        printf("  = [2]. Mo phong giao hang (animation)         =\n");
        printf("  = [3]. Tong quan kho hang (so do mat bang)    =\n");
        printf("  = [4]. Goi y duong di toi uu (BFS)            =\n");
        printf("  = [5]. Quay lai menu chinh                    =\n");
        printf("  ================================================\n\n");

        if (countWrong >= 3) {
            printf("  [!] Nhap sai qua 3 lan. Khoa tinh nang!\n");
            printf("  Nhan phim bat ky de quay lai...");
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
                printf("\n  [!] Hay nhap so tu 1 den 5.\n");
                countWrong++;
                Sleep(800);
                break;
        }
    }
    return 0;
}
