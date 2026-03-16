all:
	gcc exploit.c -o exploit --static -pthread -Wl,--start-group -lfuse3 -luring -lkeyutils -lnftnl -lmnl -Wl,--end-group -ldl 
