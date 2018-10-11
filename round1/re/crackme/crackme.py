#!/usr/bin/env python2

from pwn import *

if __name__ == "__main__":
    r = remote("challenge.matesctf.org", 9001)
    p = process("./crackme_solve")
    flag = ""
    nums = []
    i = 0
    
    for c in range(0, 2):
        r.sendlineafter("Your choice:", "1")
        r.sendlineafter("Enter your lucky number:", "1")
        r.recvuntil("Oh no, Our correct number is ")
        p.send(r.recvuntil("\n"))
        nums = nums + p.recvuntil("\n").strip().split()
        
        while True:
            r.sendlineafter("Your choice:", "1")
            r.sendlineafter("Enter your lucky number:", nums[i])
            r.recvuntil("]=")
            flag = flag + r.recvuntil("\n").strip()
            i = i + 1
            
            if i == 16 or i == 17:
                break
    
    r.sendlineafter("Your choice:", "2")
    r.sendlineafter("Enter your fake flag:", flag)
    print r.recvall()
