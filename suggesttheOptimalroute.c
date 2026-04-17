// chuc nang 3: ve ban do, tim duong bfs va mo phong shipper di chuyen
include <windows.h> // dung cho ham Sleep() tao hieu ung dung hinh

#define MAP_SIZE 30

// cau truc luu toa do x, y tren ban do
typedef struct { 
    int x, y; 
    bool delivered; 
} MapPoint;

char grid[MAP_SIZE][MAP_SIZE];       // ma tran hien thi ban do (cac ky tu)
MapPoint parent[MAP_SIZE][MAP_SIZE]; // mang luu vet duong di cua thuat toan BFS
bool visited[MAP_SIZE][MAP_SIZE];    // mang danh dau cac o da di qua trong BFS


// ham ve lai ban do ra man hinh
void displayMap() {
    printf("\033[H"); // dua con tro console ve goc tren cung ban trai (giam giat lag)
    printf("\n=== MO PHONG LO TRINH GIAO HANG ===\n");
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
    
    // in mang 2 chieu grid ra man hinh
    for (int i = 0; i < MAP_SIZE; i++) {
        printf("| ");
        for (int j = 0; j < MAP_SIZE; j++) printf("%c ", grid[i][j]); 
        printf("|\n");
    }
    for (int i = 0; i < MAP_SIZE * 2 + 3; i++) printf("-"); printf("\n");
}

// thuat toan BFS dung de tim duong di ngan nhat tranh vat can ('X')
bool find_path(MapPoint start, MapPoint end) {
    // xoa lich su lan tim kiem truoc do
    for(int i=0; i<MAP_SIZE; i++) for(int j=0; j<MAP_SIZE; j++) {
        visited[i][j] = false;
        parent[i][j] = (MapPoint){-1, -1, false};
    }
    
    // khoi tao hang doi (queue) cho BFS
    MapPoint queue[MAP_SIZE * MAP_SIZE]; 
    int head = 0, tail = 0;
    
    // dua diem xuat phat vao hang doi
    queue[tail++] = start;
    visited[start.x][start.y] = true;
    
    // 4 huong di chuyen: phai, trai, xuong, len
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0}; 

    while (head < tail) {
        MapPoint curr = queue[head++];
        
        // neu da lan den duoc diem dich -> thoat va tra ve true
        if (curr.x == end.x && curr.y == end.y) return true; 

        // thu lan rong ra 4 huong xung quanh
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            
            // neu o tiep theo nam trong ban do, chua di qua va khong phai vat can
            if (nx >= 0 && nx < MAP_SIZE && ny >= 0 && ny < MAP_SIZE && !visited[nx][ny] && grid[nx][ny] != 'X') {
                visited[nx][ny] = true;
                parent[nx][ny] = curr; // luu vet: o (nx, ny) duoc di den tu o (curr)
                queue[tail++] = (MapPoint){nx, ny, false};
            }
        }
    }
    return false; // khong the tim thay duong di toi dich
}

// ham chay animation tren console dua vao duong di da tim duoc
void move_shipper_animation(MapPoint start, MapPoint end) {
    if (!find_path(start, end)) return; // neu khong co duong thi bo qua
    
    MapPoint path[MAP_SIZE * MAP_SIZE];
    int count = 0; 
    MapPoint p = end;
    
    // truy vet nguoc tu diem dich ve lai diem xuat phat thong qua mang parent
    while (p.x != -1) { 
        path[count++] = p; 
        p = parent[p.x][p.y]; 
    }
    
    // cho shipper di chuyen tren map tu dau den cuoi
    for (int i = count - 1; i >= 0; i--) {
        int r = path[i].x, c = path[i].y;
        char backup = grid[r][c]; // luu lai ky tu tai o nay truoc khi de len
        
        grid[r][c] = '#'; // ve ky tu '#' tuong trung cho shipper dang dung tai day
        displayMap();     // update ban do
        Sleep(40);        // dung 40ms de mat nguoi nhin thay hieu ung chay
        
        // sau khi shipper di qua, hoan tra lai mat duong
        if (backup == 'S') grid[r][c] = 'S'; // giu lai vi tri bat dau
        else if (r == end.x && c == end.y) grid[r][c] = 'v'; // ve dau tich neu den noi
        else grid[r][c] = '='; // ve vet banh xe tren duong
    }
}

// ham tong hop: tim don gan nhat va hien thi animation tren ban do
void visualizeOptimalRoute(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] Thieu du lieu he thong de ve ban do!\n");
        return;
    }

    // cai dat hat giong random giong nhu o chuc nang 2
    static bool seeded_map = false;
    if (!seeded_map) {
        srand(time(NULL));
        seeded_map = true;
    }

    // chon shipper ranh dau tien trong danh sach de dai dien chay ban do
    shipper *activeShipper = *headS;
    while (activeShipper != NULL && activeShipper->status != 0) {
        activeShipper = activeShipper->next;
    }
    if (activeShipper == NULL) {
        printf("\n[Thong bao] Hien khong co shipper nao ranh!\n");
        return;
    }

    // ve nen mat duong la dau cham '.'
    for (int i = 0; i < MAP_SIZE; i++) 
        for (int j = 0; j < MAP_SIZE; j++) grid[i][j] = '.';
        
    // dat cac buc tuong vat can co dinh 'X' (ban co the chinh sua toa do neu muon)
    for (int j = 5; j < 25; j++) grid[10][j] = 'X';
    for (int j = 0; j < 20; j++) grid[20][j] = 'X';
    for (int i = 5; i < 15; i++) grid[i][25] = 'X';

    int pendingOrders = 0;
    order *currO = *headO;
    
    // quet don hang chua giao, neu nam tren mat duong thi danh dau '*' len ban do
    while (currO != NULL) {
        if (currO->status == 0 && currO->x >= 0 && currO->x < MAP_SIZE && currO->y >= 0 && currO->y < MAP_SIZE && grid[currO->x][currO->y] != 'X') {
            grid[currO->x][currO->y] = '*';
            pendingOrders++;
        }
        currO = currO->next;
    }

    if (pendingOrders == 0) {
        printf("\n[Thong bao] Khong co don hang phu hop de ve lo trinh!\n");
        return;
    }

    // lenh an con tro chuot (lam dep console khi dang ve animation)
    printf("\033[2J\033[?25l"); 
    
    // dat ky tu 'S' tuong trung cho vi tri dung hien tai cua Shipper
    grid[activeShipper->x][activeShipper->y] = 'S'; 

    // tao vong lap xu ly lan luot tung don hang
    for (int i = 0; i < pendingOrders; i++) {
        order *bestO = NULL;
        float min_dist = 1000000.0;
        
        // voi vi tri hien tai, tim xem don hang ('*') nao dang gan nhat
        currO = *headO;
        while (currO != NULL) {
            if (currO->status == 0 && grid[currO->x][currO->y] == '*') {
                float d = dist_points(activeShipper->x, activeShipper->y, currO->x, currO->y);
                if (d < min_dist) { min_dist = d; bestO = currO; }
            }
            currO = currO->next;
        }

        // tien hanh giao cho don hang gan nhat vua tim duoc
        if (bestO != NULL) {
            MapPoint start = {activeShipper->x, activeShipper->y, false};
            MapPoint end = {bestO->x, bestO->y, false};
            
            // kich hoat chay hieu ung
            move_shipper_animation(start, end);
            
            // kiem tra xem co bi khach bom hang khong
            int chance = rand() % 100;
            if (chance < 80) { // 80% giao xong xui
                bestO->status = 2; 
                grid[bestO->x][bestO->y] = 'v'; // bien ngoi sao thanh dau tick 'v'
            } else { // 20% den nha khach khong thay dau
                bestO->status = 3; 
                grid[bestO->x][bestO->y] = 'x'; // bien ngoi sao thanh dau x (bom hang)
            }
            
            // update lai vi tri moi cua shipper
            activeShipper->x = bestO->x; 
            activeShipper->y = bestO->y;
            
            // dung cho 800ms de nguoi xem kip nhin ket qua truoc khi chay di giao don tiep
            Sleep(800);
        }
    }
    
    // sau khi chay xong het, hien thi lai con tro chuot (\033[?25h)
    printf("\033[?25h\n[HOAN THANH] Lo trinh da chay xong! An Enter de thoat.");
    getchar(); getchar(); // doi nguoi dung tuong tac truoc khi thoat menu
}
