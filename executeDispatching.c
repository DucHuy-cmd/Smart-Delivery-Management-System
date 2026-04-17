// chuc nang 1: chi tim shipper gan nhat (khong lam thay doi du lieu)
void findNearestShipper(order **headO, shipper **headS) {
    // kiem tra xem he thong co du lieu chua
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] Thieu du lieu don hang hoac shipper!\n");
        return;
    }

    // nhap ma don hang tu ban phim de tim kiem
    char targetCode[50];
    printf("\nNhap ma don hang can tim shipper: ");
    scanf("%s", targetCode);
    while(getchar() != '\n'); // xoa bo nho dem ban phim

    // duyet danh sach don hang de tim don co ma vua nhap
    order *currO = *headO;
    while (currO != NULL && strcmp(currO->code, targetCode) != 0) {
        currO = currO->next;
    }

    // neu duyet het ma van la NULL -> khong thay
    if (currO == NULL) {
        printf("[Loi] Khong tim thay don hang ma: %s\n", targetCode);
        return;
    }
    // neu don hang khong o trang thai "Chua giao" (status == 0) thi bo qua
    if (currO->status != 0) {
        printf("[Thong bao] Don hang nay dang duoc giao hoac da xong!\n");
        return;
    }

    shipper *currS = *headS;
    shipper *bestS = NULL;
    float min_dist = 1000000.0; // dat khoang cach ban dau that lon

    // quet toan bo shipper de tim nguoi gan nhat
    while (currS != NULL) {
        // chi xet nhung shipper dang ranh viec (status == 0)
        if (currS->status == 0) { 
            float dist = dist_points(currO->x, currO->y, currS->x, currS->y);
            // neu tim thay shipper gan hon -> cap nhat lai shipper tot nhat
            if (dist < min_dist) { 
                min_dist = dist; 
                bestS = currS; 
            }
        }
        currS = currS->next;
    }

    // in ra ket qua
    if (bestS != NULL) {
        printf("\n=> Shipper gan nhat la: %s (Cach %.2f don vi)\n", bestS->code, min_dist);
    } else {
        printf("\n[Thong bao] Hien tai khong co shipper nao dang ranh!\n");
    }
}

// chuc nang 2: tu dong gan don va giao hang (hien thi dang van ban)
void autoAssignOrdersTextMode(order **headO, shipper **headS) {
    if (*headO == NULL || *headS == NULL) {
        printf("\n[Thong bao] Thieu du lieu he thong!\n");
        return;
    }

    // khoi tao hat giong random tai day (dung bien static de chi chay 1 lan)
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }

    order *currO = *headO;
    int count = 0; // bien dem so don hang da duoc xu ly

    printf("\n=== KET QUA TU DONG GAN DON HANG ===\n");
    
    // duyet qua toan bo danh sach don hang
    while (currO != NULL) {
        // chi xu ly nhung don chua duoc giao
        if (currO->status == 0) { 
            shipper *currS = *headS;
            shipper *bestS = NULL;
            float min_dist = 1000000.0;

            // tim shipper ranh gan voi don hang nay nhat
            while (currS != NULL) {
                if (currS->status == 0) { 
                    float dist = dist_points(currS->x, currS->y, currO->x, currO->y);
                    if (dist < min_dist) { min_dist = dist; bestS = currS; }
                }
                currS = currS->next;
            }

            // neu tim duoc shipper phu hop
            if (bestS != NULL) {
                // random ty le giao hang thanh cong (tu 0 den 99)
                int chance = rand() % 100;
                
                // neu random ra so < 80 -> tuc la 80% co hoi thanh cong
                if (chance < 80) { 
                    currO->status = 2; // cap nhat status thanh Da giao thanh cong
                    printf("- Don [%s] cho shipper [%s]: GIAO THANH CONG! (Cach: %.2f)\n", currO->code, bestS->code, min_dist);
                } else { 
                    // 20% con lai la that bai (khach bom hang, khong nghe may...)
                    currO->status = 3; // cap nhat status thanh Giao that bai (hoac ma loi ban quy dinh)
                    printf("- Don [%s] cho shipper [%s]: GIAO THAT BAI! (Khach khong nghe may)\n", currO->code, bestS->code);
                }
                
                // chot lai la du thanh cong hay that bai thi shipper van phai chay den nha khach roi
                // nen toa do hien tai cua shipper duoc cap nhat thanh toa do nha khach
                bestS->x = currO->x; 
                bestS->y = currO->y; 
                count++;
            } else {
                printf("- Don [%s] dang cho vi khong co shipper ranh!\n", currO->code);
            }
        }
        currO = currO->next; // chuyen sang don tiep theo
    }

    // neu quet het ma khong the giao duoc don nao
    if (count == 0) printf("[Thong bao] Khong co don hang nao can xu ly!\n");
}
