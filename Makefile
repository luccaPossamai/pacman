INCLUDES = includes
TARGET = pacman
NAME = run
comp: $(TARGET).c
	gcc -I$(INCLUDES) -O3 -o $(NAME) $(TARGET).c -lncurses -Wall -lm
clean:
	rm -f $(NAME)
