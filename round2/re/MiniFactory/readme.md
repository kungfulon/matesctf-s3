# MiniFactory

File nhận được là một flag checker. Để cho tiện và dễ hiểu hơn thì mình sẽ viết dựa trên source code được mình decompile từ linux binary (file `MiniFactory.cpp`). Nhấn mạnh là mã nguồn này do mình tự tay decompile, không phải là đồ đi xin xỏ :)

Bắt đầu từ hàm `main`, ta thấy chỉ có một vòng lặp vô hạn:

- Nhận vào flag do user nhập
- Gọi hàm `BinFactory::Check` để kiểm tra xem flag có đúng không

Trước khi nói về phần kiểm tra flag, mình xin nói về đám `Factory` trong bài:

- Một `Factory` có thể liên kết đến một hoặc nhiều `Factory` khác
- Với mỗi `Factory`, sau khi xử lý data, nó sẽ chia đều data cho các `Factory` mà nó liên kết đến
- Hàm `VirtualFactory::Work` có nhiệm vụ lấy data được truyền từ `Factory` trước đó bằng cách gọi hàm `VirtualFactory::GetFactoryData`, đưa vào hàm `VirtualFactory::Process` để xử lý, sau đó truyền data cho các `Factory` mà nó liên kết đến
- Hàm `VirtualFactory::Process` là một hàm `virtual`, nó có thể được override bởi các class thừa kế
- Hàm `VirtualFactory::GiveDataToNextFactory` dùng để truyền data cho các `Factory` mà nó liên kết đến

Nói nôm na thì nó trông như một mạng neuron, bạn nào học Machine Learning với Deep Learning thì chắc quá quen với kiểu này rồi (bài còn đặt tên mấy class là `InputFactory`, `HiddenFactory`, `OutputFactory` kiểu mấy loại layer vậy). Với bài này, "mạng" của chúng ta có dạng:

```
                       XOR_BIT2
                     /          \
BinFactory -- CAESAR              REVERSE -- BinFactory
                     \          / 
                       XOR_BIT3
```

Implement của các layer này khá đơn giản, các bạn có thể đọc trong code. Tại sao mình lại ra được cái sơ đồ này thì xem constructor `BinFactory::BinFactory`, trong đó có 1 đống lời gọi đến hàm `VirtualFactory::LinkFactories`, chính là để liên kết các `Factory` với nhau.

Đến với phần kiểm tra flag:

- Bắt đầu từ hàm `BinFactory::Check`, ngay đầu là truyền `flag` cho `Factory` mà `BinFactory` liên kết đến, cụ thể là `CAESAR`
- Sau đó là một vòng lặp 100 lần, với mỗi lần lặp:
    - Gọi hàm `VirtualFactory::Work` của 4 `Factory` theo thứ tự: `CAESAR`, `XOR_BIT2`, `XOR_BIT3`, `REVERSE`
    - Sau khi 4 `Factory` "làm việc" xong, `BinFactory` lấy `data` đã được xử lý. Nếu `size` của `data` lớn hơn `0`, ra khỏi vòng lặp.
- Nếu `size` của data lớn hơn `0`, so sánh `size` với `MAGIC_SIZE`, nếu bằng thì tiếp tục kiểm tra so khớp `data` với `MAGIC`. Nếu khớp, thông báo chúc mừng và trả về `true`
- Tất cả các trường hợp còn lại, thông báo không khớp và trả về `false`

Có 1 chỗ khá hay trong bài này, đó là tác giả nhân 8 lần size lên để xử lý, và trong hàm lấy / truyền data, có đoạn kiểm tra bất kỳ 3 bits cuối của tổng size đã lấy / truyền có bật hay không, nếu có thì encode / decode, không thì chỉ gán đơn thuần (cách encode / decode xin mời các bạn đọc code).

Sau khi phân tích và nắm rõ flow của chương trình, mình tiến hành đảo ngược quy trình để lấy flag (file `Solve.cpp`). `XOR_BIT2` và `XOR_BIT3` làm mất dữ liệu nên mình sử dụng brute force byte-by-byte để đảo ngược. Flag của challenge là:

```
matesctf{1AA03492CB9FC65FA6B9E98077B6DC23}
```

## Trivia

Mình bắt đầu làm bài này vào buổi chiều, khi đã có đội first blood được rồi. Trong lúc thi, mình bỏ qua mất cái `FactoryData` và hàm `VirtualFactory::Work`. Mình tưởng là hàm `BinFactory::Check` chỉ gọi `VirtualFactory::Process` của 4 cái `Factory` kia thôi nên mãi không thấm việc lấy / truyền dữ liệu của các `Factory`. Mình quyết định vất vào GDB, break tại mấy hàm `VirtualFactory::Process` đã override của 4 `Factory` kia để theo dõi sự biến đổi của data, sau đó hiểu được flow và đảo ngược. Đến sau khi thi xong mới phát hiện ra là mình đã đốt cháy giai đoạn.
