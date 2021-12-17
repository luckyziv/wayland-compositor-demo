WAYLAND = -lwayland-server
CFLAGS = -lrt
FF = compositor-demo.c
CC = gcc

# INCLUDE = -I/usr/include/wlr
LIB_PATH = -L/usr/lib//usr/lib/x86_64-linux-gnu/ -lwlroots
#libwlroots.so

compositor-demo: $(FF)
	$(CC) -o compositor-demo $(FF) $(WAYLAND) $(CFLAGS) $(INCLUDE) $(LIB_PATH)

.PHONY: clean
clean:
	$(RM) *.o compositor-demo
