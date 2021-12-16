WAYLAND = -lwayland-server
CFLAGS = -lrt
FF = compositor-demo.c
CC = gcc

compositor-demo: $(FF)
	$(CC) -o compositor-demo $(FF) $(WAYLAND) $(CFLAGS)

.PHONY: clean
clean:
	$(RM) *.o client