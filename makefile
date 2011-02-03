
all:
	make -C utility all
	make -C display all
	make -C client all

clean:
	make -C utility clean
	make -C display clean
	make -C client clean
