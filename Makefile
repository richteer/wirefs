#Written by Chris Harding
SHELL   = /bin/bash
TARGET  = wirefs
CC      = gcc
CFLAGS  = -c
LDFLAGS = -lm `pkg-config fuse --libs --cflags`

SRCS := bitmap.c  blocks.c  crash.c  dir.c  file.c  inode.c  sb.c util.c
DEPS := $(addprefix dep/,$(patsubst %.c,%.d,$(SRCS)))
OBJS := $(addprefix obj/,$(patsubst %.c,%.o,$(SRCS)))
SRCS := $(addprefix src/,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) fs.c
	$(CC) $(OBJS)  $(LDFLAGS) fs.c -o $(TARGET)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	$(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf obj
	rm -rf dep
	rm -f $(TARGET)

mount: $(TARGET) disk.img
	./$< --disk disk.img -d test --no-crash

umount:
	fusermount -u test

format: $(TARGET) disk.img
	./$< --disk disk.img --format 409600

disk.img:
	truncate --size 200M disk.img

-include $(DEPS)
