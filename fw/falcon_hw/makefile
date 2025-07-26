CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O2 -I.
TARGET = falcon_test.elf

SRCS = pqm4.c codec.c common.c fft.c randombytes.c fpr.c keygen.c sign.c vrfy.c rng.c fips202.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) -lm

clean:
	rm -f $(TARGET) *.o