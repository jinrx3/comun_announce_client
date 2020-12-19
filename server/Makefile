CC			=	gcc
ELF 		= 	server
SRC 		= $(wildcard ./*.c) 

OBJ			= $(patsubst %.c,%.o,$(SRC))


$(ELF):$(OBJ)
	$(CC) $^ -o $@ $(CONFIG) 

%.o:%.c
	$(CC) $< -o $@  -c $(CONFIG)

clean:
	$(RM) $(ELF) $(OBJ)