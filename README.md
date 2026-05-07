# Hệ Thống Quản Lý Giao Hàng Thông Minh

Hệ thống quản lý giao hàng thông minh sử dụng thuật toán BFS để tìm đường ngắn nhất,
viết bằng ngôn ngữ C, chạy trên Windows Console.

---

## Yêu cầu hệ thống

- Hệ điều hành : Windows 7 / 10 / 11 (bắt buộc, không hỗ trợ Linux hay macOS)
- Trình biên dịch: GCC (MinGW-w64), Code::Blocks, Dev-C++, hoặc Visual Studio
- Chuẩn ngôn ngữ: C99 trở lên
- Font console  : Consolas hoặc Courier New

---

## Cài đặt môi trường

### Cách 1: Dùng Code::Blocks (Khuyến nghị)

1. Tải tại https://www.codeblocks.org/downloads/
   Chọn bản "codeblocks-XX.XXmingw-setup.exe" vì đã kèm sẵn MinGW.
2. Mở Code::Blocks > File > New > Project > Console Application > chọn C.
3. Chuột phải vào project > Add files > chọn tất cả file nguồn.
4. Vào Project > Build options > Compiler settings > Other options > thêm: -std=c99
5. Nhấn F9 để biên dịch và chạy.

### Cách 2: Dùng Dev-C++

1. Tải tại https://sourceforge.net/projects/orwelldevcpp/
2. Vào File > New > Project > Console Application > chọn C.
3. Project > Add to project > chọn tất cả file nguồn.
4. Tools > Compiler Options > thêm: -std=c99 -lm
5. Nhấn F11 để biên dịch và chạy.

### Cách 3: Dùng GCC trên Command Prompt

1. Tải MinGW-w64 tại https://www.mingw-w64.org/downloads/
2. Thêm thư mục bin vào biến môi trường PATH. Ví dụ: C:\mingw64\bin
3. Kiểm tra: gcc --version
4. Di chuyển đến thư mục chứa file nguồn: cd C:\...\SmartDelivery
5. Chạy lệnh biên dịch:
   gcc -std=c99 -o SmartDelivery Main.c Functions.c "OPTION 1.c" "OPTION 2.c" "OPTION 3.c" "OPTION 4.c" -lm
6. Chạy chương trình: SmartDelivery.exe

---

```
SmartDelivery/
├── Main.c        <- Điểm khởi động chương trình
├── Lib.h         <- Khai báo cấu trúc dữ liệu và prototype hàm
├── Functions.c   <- Hàm tiện ích dùng chung, đọc/ghi file, menu chính
├── OPTION 1.c    <- Module quản lý đơn hàng
├── OPTION 2.c    <- Module quản lý shipper
├── OPTION 3.c    <- Module điều phối thông minh (thuật toán BFS)
├── OPTION 4.c    <- Module thống kê và báo cáo
└── README.md     <- File hướng dẫn này

Các file dữ liệu tự động tạo khi chạy:
├── Order Information.txt      <- Lưu trữ đơn hàng
├── Shipper information.txt    <- Bảng thông tin shipper
├── Shipper data.txt           <- Dữ liệu shipper dạng raw
├── route report.txt           <- Báo cáo tuyến đường BFS
├── warehouse overview.txt     <- Tổng quan kho hàng
└── Compile Report.txt         <- Báo cáo thống kê cuối ngày
```

---

## Lưu ý trước khi chạy chương trình

- Chỉ chạy được trên Windows.
- Nếu ký tự khung menu bị vỡ: chuột phải thanh tiêu đề console > Properties > Font > chọn Consolas.
- Nếu GCC báo lỗi "undefined reference to N0S": kiểm tra Functions.c đã được thêm vào lệnh biên dịch chưa.
- Nếu không tạo được file dữ liệu: thử chạy Command Prompt bằng quyền Administrator.

---

## Chức năng chính

- Quản lý đơn hàng  : thêm, tìm kiếm, sắp xếp theo ngày, cập nhật thông tin
- Quản lý shipper   : đăng ký, xóa, xem danh sách
- Điều phối thông minh: BFS tìm đường ngắn nhất, điều phối tự động, animation giao hàng
- Thống kê báo cáo  : tính doanh thu, xuất báo cáo ra file văn bản
