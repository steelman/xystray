LDFLAGS=-lX11 -lXt  -L/usr/X11/lib
xystray: Xystray.o xystray.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm Xystray.o xystray.o
