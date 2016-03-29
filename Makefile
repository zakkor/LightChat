release:
	cd Server && make release
	cd Client && make release
clean:
	cd Server && make clean
	cd Client && make clean
