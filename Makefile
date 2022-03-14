SRC := 	aac_adts.c   \
		aac_adts.h   \
		h264_nalu.c  \
		h264_nalu.h  \
		flv_format.h \
		flv.c flv.h  \
		main.c

TARGET := flv_mux_h264_aac

CC := gcc
CFLAGS := -g -Wall
LDFLAGS :=

ifeq ($(DEBUG), 1)
CFLAGS += -DENABLE_DEBUG
endif


$(TARGET) : $(SRC)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean :
	rm -rf $(TARGET) out*
.PHONY := clean
