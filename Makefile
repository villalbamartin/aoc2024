CC=gcc
CFLAGS=`pkg-config --cflags glib-2.0` -Wall -pedantic
LIBS=`pkg-config --libs glib-2.0`	

all: day1 day2 day3 day4 day5 day6
	echo "Making all"

day1: day1.o
	$(CC) $(CFLAGS) -g -o $@ $< $(LIBS)

day2: day2.o
	$(CC) $(CFLAGS) -g -o $@ $< $(LIBS)

day3: day3.o
	$(CC) $(CFLAGS) -g -o $@ $< $(LIBS)

day4: day4.o
	$(CC) $(CFLAGS) -g -o $@ $< $(LIBS)

day5: day5.o
	$(CC) $(CFLAGS) -g -o $@ $< $(LIBS)

day6: day6.o utils.o
	$(CC) $(CFLAGS) -g -o $@ $< utils.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -g -c $< $(LIBS)

%.c:
	touch $@

clean:
	rm -f *.o day?
