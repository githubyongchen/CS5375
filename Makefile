CC = gcc
OBJ = cachesim 

$(OBJ):$(OBJ).o
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c -o $@ $^

clean: 
	rm -f $(OBJ) *.o
