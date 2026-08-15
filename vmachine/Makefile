all:
	gcc src/exploit.c src/snd_mwp.c src/route4_df.c src/utils.c -o exploit -g -ggdb -no-pie --static -pthread -O2 -Wl,--start-group -lfuse3 -luring -lkeyutils -lnftnl -lmnl -Wl,--end-group -ldl
	gcc test.c    -o test    --static
	#gcc exploit.c -o exploit --static -pthread -Wl,--start-group -lfuse3 -luring -lkeyutils -lnftnl -lmnl -Wl,--end-group -ldl

