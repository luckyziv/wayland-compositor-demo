WAYLAND = -lwayland-server
CFLAGS = -lrt
FF = compositor-demo.c
CC = gcc
INC_CLUDE = -I/usr/include/wlr

compositor-demo: $(FF)
	$(CC) -o compositor-demo $(FF) $(WAYLAND) $(CFLAGS) $(INCLUDE)

.PHONY: clean
clean:
	$(RM) *.o compositor-demo
