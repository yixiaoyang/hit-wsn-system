#
# uart1  测试
# xiaoyang yi@2011.4.24
#

#CC = arm-linux-gcc
#LD = arm-linux-ld

CC = gcc
LD = ld

EXEC = uart1 
OBJS = uart1.o 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lpthread
	rm -f *.o

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
