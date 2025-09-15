CC = gcc
CFLAGS = -Wall -std=c99 -O2
LDFLAGS = -lraylib -lm -lpthread -ldl -lX11

SRCS = main.c models.c chess_logic.c chess_ai.c
OBJS = $(SRCS:.c=.o)
TARGET = vortexmate

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)