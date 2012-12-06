confine:main.o
	$(CC) $< -o $@


install:
	install -o root -g root confine /usr/bin/confine
	chmod +s /usr/bin/confine
