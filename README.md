# BÁO CÁO THỰC HÀNH BUỔI 4 (TH4)
**Môn học:** Thí nghiệm Khoa học Kỹ thuật 2 (AET2022)  
**Sinh viên thực hiện:** Phạm Minh Hoàng  
**Mã sinh viên:** 25112214  
**Thiết bị:** Heltec ESP32-S3 WiFi Kit V3  

---

## 1. TỔNG QUAN VÀ KẾT QUẢ THỰC HÀNH

### **Bài 1: REST API - Route GET /api/sensor Trả JSON**
- **Đường dẫn (Endpoint):** `http://<IP_ESP32>/api/sensor`
- **Định dạng dữ liệu:** `application/json` (dành cho MÁY đọc).
- **Trường dữ liệu tùy biến thêm:** `"id": "25112214"` và `"rssi": WiFi.RSSI()`.
- **Kết quả thu được:** Trình duyệt / curl hiển thị chuỗi JSON chuẩn:
  `{"temp":27.8,"hum":61.2,"uptime":45,"id":"25112214","rssi":-58}`

### **Bài 2: MQTT Publish Telemetry Theo Group Namespace**
- **Topic Telemetry:** `aet2022/nhom-25112214/telemetry`
- **Broker:** `broker.hivemq.com` (cổng 1883).
- **Tốc độ đẩy:** Định kỳ 2 giây/lần.
- **Trường bổ sung:** `"rssi"` tín hiệu Wi-Fi và `"id"` nhận diện nhóm.

### **Bài 3: Node-RED Dashboard & Giao Tiếp Hai Chiều**
- **Chiều đi (ESP32 -> Node-RED):** ESP32 Publish số đo JSON lên `aet2022/nhom-25112214/telemetry`. Node-RED đọc dữ liệu và vẽ lên Gauge nhiệt độ / Biểu đồ độ ẩm.
- **Chiều về (Node-RED -> ESP32):** Nhấn nút `BẬT LED` / `TẮT LED` trên Dashboard sẽ Publish tin nhắn `'1'` hoặc `'0'` tới Topic `aet2022/nhom-25112214/cmd/led`. ESP32 Subscribe topic này và điều khiển bật/tắt LED chân GPIO14.
- **File Flow:** Lưu tại `nodered-flow/flow.json`.

### **Bài 4: Kiểm Tra Bằng MQTT Explorer**
- **Wildcard Topic:** Subscribe `aet2022/#` để kiểm tra cây dữ liệu toàn lớp.
- **Xác nhận:** Topic của nhóm `aet2022/nhom-25112214/...` tách biệt hoàn toàn với nhóm khác, gói JSON telemetry hiển thị đúng các thuộc tính và nhận tin nhắn `cmd/led` tức thì khi tương tác trên Node-RED.

---

## 2. HÌNH ÁNH / VIDEO MINH CHỨNG THỰC TẾ

![Bài 1 - REST API JSON](media/bai1_rest_json.png)
*Hình 1: Kết quả gọi API GET /api/sensor trả dữ liệu dạng application/json trên trình duyệt.*

![Bài 2 - MQTT Telemetry](media/bai2_mqtt_pub.png)
*Hình 2: Số đo telemetry đẩy định kỳ 2s lên topic aet2022/nhom-25112214/telemetry.*

![Bài 3 - Node-RED Dashboard](media/bai3_nodered_dashboard.png)
*Hình 3: Giao diện Node-RED Dashboard với Gauge, Chart và nút bấm điều khiển hai chiều.*

![Bài 4 - MQTT Explorer Tree](media/bai4_mqtt_explorer.png)
*Hình 4: Cây topic aet2022/nhom-25112214 trên MQTT Explorer xác nhận đúng namespace và payload.*

---

## 3. TRẢ LỜI CÂU HỎI ÔN TẬP VẤN ĐÁP (SLIDE 23 / 83)

1. **REST trả JSON khác trả HTML (buổi TH3) ở điểm nào? Vì sao JSON tiện cho MÁY đọc?**
   - **Khác biệt:** HTML chứa giao diện trực quan (thẻ `<h1>`, `<button>`) phục vụ cho NGƯỜI xem trên trình duyệt. JSON chỉ chứa cấu trúc dữ liệu thuần túy (Key-Value) ngắn gọn dành cho MÁY (Node-RED, Mobile App, Server) đọc.
   - **Lý do tiện lợi:** JSON có dung lượng nhẹ, định dạng chuẩn hóa giúp mã lập trình ở phía máy chủ/ứng dụng dễ dàng bóc tách (parse) thành biến dữ liệu mà không cần phải phân tích cú pháp chuỗi giao diện phức tạp.

2. **Trong REST, bên nào khởi tạo trao đổi (client hay server)? Trong MQTT, bên nào khởi tạo?**
   - **REST:** Client luôn là bên chủ động gửi yêu cầu (Request), Server đóng vai trò thụ động chờ và trả phản hồi (Response).
   - **MQTT:** Cả Client gửi (Publisher) và Client nhận (Subscriber) đều chủ động kết nối tới Broker. Khi có dữ liệu mới, Publisher tự đẩy (Push) tới Broker để chuyển tới Subscriber mà không cần Subscriber phải hỏi liên tục.

3. **Namespace topic `aet2022/<sv_id>/telemetry` giải quyết vấn đề gì khi nhiều nhóm dùng chung một broker?**
   - Giúp **tách biệt vùng dữ liệu (isolation)** giữa các thiết bị. Việc gán mã nhóm/SV vào tiền tố đường dẫn topic giúp tránh tình trạng đè dữ liệu, xung đột bản tin hoặc nhận nhầm lệnh điều khiển giữa các nhóm khi dùng chung một Broker công cộng.

4. **Trong flow Node-RED của bạn: node nào lo chiều ĐI (thiết bị -> dashboard), node nào lo chiều VỀ (dashboard -> thiết bị)?**
   - **Chiều ĐI (Thiết bị -> Dashboard):** Node `mqtt in` (lắng nghe topic telemetry) -> Node `function` (bóc tách dữ liệu JSON) -> Các node hiển thị `ui_gauge` và `ui_chart`.
   - **Chiều VỀ (Dashboard -> Thiết bị):** Các node nút bấm `ui_button` (bắt sự kiện bấm chuột) -> Node `mqtt out` (publish lệnh '1'/'0' xuống topic cmd/led).

5. **Vì sao phải gọi `mqtt.loop()` thường xuyên và đặt client id khác nhau cho mỗi kit?**
   - **Gọi `mqtt.loop()` thường xuyên:** Để duy trì thói quen gửi gói tin giữ kết nối (keep-alive/ping) với Broker, đồng thời kiểm tra và xử lý kịp thời các bản tin ngắt từ Broker gửi về hàm callback.
   - **Client ID duy nhất:** Broker dùng Client ID để quản lý phiên kết nối. Nếu hai kit dùng chung một Client ID, Broker sẽ liên tục đá (disconnect) thiết bị cũ ra để nhận thiết bị mới, gây ra hiện tượng mất kết nối liên tục (reconnect loop).

6. **Đề xuất lựa chọn giao thức cho 2 trường hợp cụ thể:**
   - *(a) Lấy số đo một lần khi người dùng bấm nút trên web:* Dùng **REST API** (mô hình Request-Response tối ưu cho các thao tác truy xuất dữ liệu theo yêu cầu đơn lẻ, tránh lãng phí băng thông).
   - *(b) Vẽ biểu đồ nhiệt độ cập nhật liên tục cho nhiều phòng:* Dùng **MQTT** (mô hình Publish-Subscribe thời gian thực giúp thiết bị tự động đẩy số đo định kỳ mượt mà, tối ưu cho việc vẽ biểu đồ theo dõi liên tục).
