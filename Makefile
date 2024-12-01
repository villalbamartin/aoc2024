CFLAGS=`pkg-config --cflags glib-2.0`
LIBS=`pkg-config --libs glib-2.0`	

day1: day1.o
	$(CC) $(CFLAGS) -g -o day1 day1.o $(LIBS)
	
day1.o: day1.c
	$(CC) $(CFLAGS) -g -c day1.c $(LIBS)

day1.c:
	touch day1.c
clean:
	rm *.o day1
