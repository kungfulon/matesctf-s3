# Guess and win

Tác giả học dốt văn nên viết câu cú rất lủng củng và có phần khó hiểu. Nếu có bất kỳ thắc mắc gì xin hãy xem code hoặc tạo issues. Xin chân thành cảm ơn.

Load file vào IDA, tìm các string ```Generate fake flag...``` và ```Enter your lucky number```, ta thấy:

- ```sub_400ED0``` là hàm sinh fake flag
- ```sub_401070``` là hàm kiểm tra lucky number

Dùng Hex-Rays decompile hàm kiểm tra lucky number, ta thấy 1 loạt các chỉ thị SSE để sinh ra các số này. Có một số điều được rút ra:

- ```xmmword_401720``` nằm trong section ```.rodata``` và có data sẵn
- ```dword_602340``` là biến đếm để lấy data từ ```xmmword_401720```, mỗi lần gen số sẽ tăng 1 và mod 17 => ```xmmword_401720``` là 1 mảng 17 số 16 bytes
- ```xmmword_602360``` và ```xmmword_602370``` cũng góp phần sinh ra các lucky number, nhưng nằm trong section ```.bss``` => chúng được khởi tạo trong runtime
- ```dword_6023AC``` là biến đếm số lần trả lời của user, bắt đầu từ 0, khi đến 16 sẽ được gán lại về 0 và trigger đoạn code khởi tạo lại random seed và ```xmmword_602360```. Trong vòng while khởi tạo, ta thấy rằng con trỏ chạy từ ```xmmword_602360``` đến ```dword_602380``` thì dừng, mỗi lần tăng 2 bytes => ```xmmword_602360``` và ```xmmword_602370``` thực chất là 1 mảng 2 số 16 bytes
- ```dword_602380``` là biến đếm để trả về từng ký tự của fake flag khi user trả lời đúng. Khi ```dword_602380``` lớn hơn 16, chương trình sẽ luôn báo user trả lời sai mặc dù nhập số đúng => fake flag gồm 17 ký tự

Sang hàm tạo fake flag, ta thấy đoạn khởi tạo lại random seed và ```xmmword_602360``` cũng xuất hiện. Có thêm một vài quan sát:

- Chương trình lấy seed ban đầu từ ```/dev/urandom```, nếu không được thì lấy ```time(0)``` làm seed
- Fake flag gồm các ký tự từ ```a``` đến ```z```
- ```dword_6023A4``` được dùng trong phần khởi tạo lại random seed, và nó được gán giá trị lần đầu là ```rand()``` với seed ban đầu. ```byte_6023C4``` cũng tham gia vào phần khởi tạo lại, và được gán giá trị lần đầu là 0. Sau khi khởi tạo ```xmmword_602360```, ```byte_6023C4``` được gán giá trị ```rand()```

Nhìn vào lời gọi hàm ```srand()``` trong phần khởi tạo lại random seed:

``` cpp
srand((unsigned short)dword_6023A4 * (byte_6023C4 & 0xF) + (byte_6023C4 >> 4) + ((unsigned int)dword_6023A4 >> 16));
```

Với giá trị ban đầu của ```byte_6023C4``` là ```0```, lời gọi này sẽ thành:

``` cpp
srand((unsigned int)dword_6023A4 >> 16));
```

Ồ, vậy là seed của lần khởi tạo đầu tiên chỉ là 16 bit đầu của ```dword_6023A4```, tức là từ ```0``` đến ```65535```. Vì khả năng trùng số là rất nhỏ, nên ta có thể nhập bừa số đầu tiên để lấy số đúng đầu tiên của chương trình, sau đó brute force seed, với mỗi seed chạy hàm tính toán lucky number, đến khi gặp số đầu tiên mà trước đó ta nhận được thì dừng. Để ý nhận xét bên trên rằng sau 17 lần đoán thì chương trình sẽ chạy khởi tạo lại random seed và ```xmmword_602360```. Sinh 16 số còn lại của cycle này và nhập vào chương trình để lấy 16 ký tự đầu tiên của fake flag.

Vậy là ta có 16 bit đầu của ```dword_6023A4```. Ta có thể brute force tiếp 16 bit sau. Lại tiếp tục nhập bừa số để lấy số đúng đầu tiên của vòng mới, sau đó brute force 16 bit sau, với mỗi seed chạy phần khởi tạo 2 lần sau đó chạy hàm tính toán lucky number đến khi gặp số trước đó ta nhận được. Sinh ra 1 số tiếp theo của cycle này và nhập vào chương trình để lấy ký tự cuối cùng của fake flag.

Nhập fake flag vào chương trình, ta nhận được:

```
Your point: 1261
This is real flag: matesctf{Do_you_use_SMT_solver}

This is awesome flag: matesctf{null_byte_kills_PRNG}
```

Toàn bộ quy trình tính toán lucky number nằm trong file ```crackme_solve.cpp``` (dòng lệnh compile nằm ở comment đầu file). Quy trình giao tiếp với chương trình nằm trong file ```crackme.py```.
