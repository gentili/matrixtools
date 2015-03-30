all:
	make -C utility all
	make -C display all
	make -C mtop all

clean:
	make -C utility clean
	make -C display clean
	make -C mtop clean
