CC = msdk-linux-gcc
OPENSSL_DIR ?= $(HOME)/openssl-1.0.2d

TARGET := MqttComm
OUTDIR ?= fw_data
OBJDIR ?= build
TARGET_PATH := $(OUTDIR)/$(TARGET)

CPPFLAGS += -I. -I./include -I$(OPENSSL_DIR)/include -D MQTT_USE_BIO
CFLAGS += -g -Wall -Os -MMD -MP
LDFLAGS += -L$(OPENSSL_DIR)
LDLIBS += -lpthread -lssl -ldl -lcrypto -lm

SRCS := cJSON.c MqttComm.c mqtt.c mqtt_pal.c create_objects.c \
        ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean distclean MqttComm list-unused

all: $(TARGET_PATH)

MqttComm: $(TARGET_PATH)

list-unused:
	@echo "C sources not used by $(TARGET):"
	@for f in $(filter-out $(SRCS),$(notdir $(wildcard *.c))); do echo "  $$f"; done

$(OUTDIR) $(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET_PATH): $(OBJS) libapmib.so | $(OUTDIR)
	$(CC) $(LDFLAGS) $(OBJS) libapmib.so $(LDLIBS) -o $@

-include $(DEPS)

clean:
	@echo "Cleaning build outputs..."
	$(RM) $(OBJS) $(DEPS) $(TARGET_PATH)
	@rmdir $(OBJDIR) 2>/dev/null || true
	@echo "Done."

distclean: clean
	@rmdir $(OUTDIR) 2>/dev/null || true
