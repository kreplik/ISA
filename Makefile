CC = g++
CFLAGS = -Wall -Werror -pedantic -std=c++11
LIBS = -lpcap
OBJS = dns.o

TARGET = dns
SRCS = $(TARGET).cpp

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

test:
	python3 test.py

clean:
	rm -f $(TARGET)