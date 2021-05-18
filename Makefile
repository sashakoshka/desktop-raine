CC=gcc
CFLAGS=-Wall
LIBS=-L/usr/local/lib -lSDL2 -lSDL2_image

raine:
	$(CC) main.c -o raine $(CFLAGS) $(LIBS)

run: clean raine
	./raine
	make clean

clean:
	rm -f raine raine~

install: raine
	install -d /usr/bin/
	install -d /usr/share/applications/
	install -d /usr/share/icons/desktop-raine
	
	install -m 755 raine /usr/bin/raine
	install -m 755 raine.desktop /usr/share/applications/raine.desktop
	install -m 755 img/* /usr/share/icons/desktop-raine/
