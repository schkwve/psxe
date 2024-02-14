TARGET := psxe
CFILES := $(shell find . -name "*.c")
OBJ := $(CFILES:%.c=build/%.o)
CFLAGS := -Wall -Wextra -Wpedantic

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) -o $@

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)
