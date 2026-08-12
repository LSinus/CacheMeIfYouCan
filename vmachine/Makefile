all:
	gcc src/exploit.c src/snd_mwp.c src/route4_df.c -o exploit --static -pthread -O2 -s -Wl,--start-group -lfuse3 -luring -lkeyutils -lnftnl -lmnl -Wl,--end-group -ldl
	gcc test.c    -o test    --static
	#gcc exploit.c -o exploit --static -pthread -Wl,--start-group -lfuse3 -luring -lkeyutils -lnftnl -lmnl -Wl,--end-group -ldl

