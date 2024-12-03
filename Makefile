CFLAGS=`pkg-config --cflags glib-2.0`
LIBS=`pkg-config --libs glib-2.0`	

day1: day1.o
	$(CC) $(CFLAGS) -g -o day1 day1.o $(LIBS)

day2: day2.o
	$(CC) $(CFLAGS) -g -o day2 day2.o $(LIBS)
	
day1.o: day1.c
	$(CC) $(CFLAGS) -g -c day1.c $(LIBS)

day2.o: day2.c
	$(CC) $(CFLAGS) -g -c day2.c $(LIBS)

day1.c:
	touch day1.c

day2.c:
	touch day2.c

clean:
	rm *.o day1 day2
