# 如果您的編譯器路徑與範例不同，請修改這裡；若已在 PATH 中，可直接設為 msdk-linux-gcc
# TOOLCHAIN = /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b
# CC        = $(TOOLCHAIN)/bin/msdk-linux-gcc
CC        = msdk-linux-gcc

# OpenSSL 路徑 (將 ~ 替換為 $(HOME) 以確保路徑正確)
OPENSSL_DIR = $(HOME)/openssl-1.0.2d

# CFLAGS: 包含 -Os, -g, -Wall, 定義 MQTT_USE_BIO 以及 include 路徑
CFLAGS    = -I. -I$(OPENSSL_DIR)/include -I./include \
            -g -Wall -Os -D MQTT_USE_BIO \
            -MMD -MP

# LDFLAGS: OpenSSL 的 lib 路徑
LDFLAGS   = -L$(OPENSSL_DIR)

# LDLIBS: 連結所需的函式庫 (-lm, -lpthread, -lssl 等)
LDLIBS    = -lpthread -lssl -ldl -lcrypto -lm

OUTDIR    = fw_data
OBJDIR    = build

# 所有來源檔案
SRCS      = cJSON.c MqttComm.c mqtt.c mqtt_pal.c create_objects.c \
            ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c

# 將 .c 替換為 .o 並加上 OBJDIR 前綴
OBJS      = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

# 主要目標
all: MqttComm

# 目錄自動建立
$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# 一般 .c -> .o (輸出到 build/)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ===== 可執行檔 =====
# 注意：這裡依照您的指令，將 libapmib.so 直接放入連結命令中
MqttComm: $(OBJS) | $(OUTDIR)
	$(CC) $(LDFLAGS) $^ libapmib.so $(LDLIBS) -o $(OUTDIR)/MqttComm

# 自動依賴 (由 -MMD -MP 產生)
-include $(OBJDIR)/*.d

.PHONY: clean
clean:
	@echo "Cleaning object files and build outputs..."
	@find $(OBJDIR) -type f \( -name "*.o" -o -name "*.d" \) -delete 2>/dev/null || true
	@find $(OUTDIR) -type f -maxdepth 1 -delete 2>/dev/null || true
	@echo "Done."