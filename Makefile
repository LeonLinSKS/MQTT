TOOLCHAIN = /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b
CC       = $(TOOLCHAIN)/bin/msdk-linux-gcc
OPENSSL_DIR ?= $(HOME)/openssl-1.0.2d

TARGET := MqttComm
OUTDIR ?= fw_data
OBJDIR ?= build
TARGET_PATH := $(OUTDIR)/$(TARGET)

CPPFLAGS += -I. -I./include -I$(OPENSSL_DIR)/include -D MQTT_USE_BIO
CFLAGS += -g -Wall -Os -MMD -MP
LDFLAGS += -L$(OPENSSL_DIR)
LDLIBS += -lpthread -lssl -ldl -lcrypto -lm

SRCS := src/cJSON.c src/MqttComm.c src/mqtt.c src/mqtt_pal.c src/create_objects.c \
        src/ReadZoneMerge.c src/ReadZoneStatus.c src/ReadFlash.c src/RealTime.c src/LogManage.c
OBJS := $(SRCS:src/%.c=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean distclean MqttComm list-unused

all: $(TARGET_PATH)

MqttComm: $(TARGET_PATH)

list-unused:
	@echo "C sources not used by $(TARGET):"
	@for f in $(filter-out $(SRCS),$(notdir $(wildcard *.c))); do echo "  $$f"; done

$(OUTDIR) $(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET_PATH): $(OBJS) lib/libapmib.so | $(OUTDIR)
	$(CC) $(LDFLAGS) $(OBJS) lib/libapmib.so $(LDLIBS) -o $@

-include $(DEPS)

clean:
	@echo "Cleaning build outputs..."
	$(RM) $(OBJS) $(DEPS) $(TARGET_PATH)
	@rmdir $(OBJDIR) 2>/dev/null || true
	@echo "Done."

distclean: clean
	@rmdir $(OUTDIR) 2>/dev/null || true
