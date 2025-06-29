INCLUDES = includes
TARGET = pacman
NAME = run
comp: $(TARGET).c
		gcc -I$(INCLUDES) -g -o $(NAME) $(TARGET).c -lncurses -Wall -lm

clean:
	rm -f $(NAME)
