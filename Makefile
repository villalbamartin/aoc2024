CFLAGS=`pkg-config --cflags glib-2.0`
LIBS=`pkg-config --libs glib-2.0`	

all: day1 day2
	echo "Making all"

day1: day1.o
	$(CC) $(CFLAGS) -g -o day1 day1.o $(LIBS)

day2: day2.o
	$(CC) $(CFLAGS) -g -o day2 day2.o $(LIBS)

day3: day3.o
	$(CC) $(CFLAGS) -g -o day3 day3.o $(LIBS)
	
%.o: %.c
	$(CC) $(CFLAGS) -g -c $< $(LIBS)

%.c:
	touch $@

clean:
	rm *.o day1 day2
