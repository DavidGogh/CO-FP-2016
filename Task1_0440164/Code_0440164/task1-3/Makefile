CC	= ../riscv/bin/riscv64-unknown-elf-gcc
CC-HOST	= gcc
TARGET	= bns-riscv
TARGET-HOST	= bns
SPIKE	= ../riscv/bin/spike
SPIKE-OPTION	= --ic=8:2:256 --dc=1:16:256 --l2=1:1024:8 pk
CHECKER	= checker

bns-riscv: bns.c checker
	$(CC) -o $@ $<

checker: checker.c
	$(CC-HOST) -o $@ $<

.PHONY: clean host run run-host
clean:
	rm -fr $(TARGET) $(TARGET-HOST) $(CHECKER) ./Output/*

host: bns.c bmp.h checker
	$(CC-HOST) -o $(TARGET-HOST) $<

run: 
	$(SPIKE) $(SPIKE-OPTION) $(TARGET)
	./$(CHECKER)

run-host: 
	./$(TARGET-HOST)
	./$(CHECKER)

run-best:
	$(SPIKE) --ic=8:2:256 --dc=1:16:256 --l2=1:1024:8 pk $(TARGET)
	./$(CHECKER)

run-testL2:
	 for i in 1 2 4 8 16 32 64 128 256 512 1024; do \
		for j in 1 2 4 8 16 32 64 128 256 512 1024; do \
			echo $$i ; \
			echo $$j ; \
			echo $$((8192/$$i/$$j)) ; \
			$(SPIKE) --ic=8:2:256 --dc=1:16:256 --l2=$$i:$$j:$$((8192/$$i/$$j)) pk $(TARGET); \
		done ; \
	done
