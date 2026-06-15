# Standalone test harness for Urban Terror Optimized.
#
# Lives outside the engine build (and outside the upstream-aligned code/ tree):
# it compiles individual, dependency-light qcommon translation units against the
# Unity framework. Runs under ASan+UBSan by default so tests double as a
# memory/UB check.
#
#   make -C tests            # build + run unit tests
#   make -C tests unit       # same
#   make -C tests fuzz       # build libFuzzer targets (needs clang)
#   make -C tests clean
#
# Override the compiler with CC=clang / CC=gcc.

CC      ?= cc
ROOT    := $(CURDIR)/..
QCOMMON := $(ROOT)/code/qcommon
UNITY   := unit/vendor/unity
OUT     := build

SAN      := -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS   := -std=gnu99 -g -O1 -Wall -I$(UNITY) -I$(QCOMMON) $(SAN)
LDFLAGS  := $(SAN) -lm

UNIT_BIN := $(OUT)/run_unit

.PHONY: all unit fuzz clean
all: unit

unit: $(UNIT_BIN)
	@echo "=== running unit tests ==="
	@./$(UNIT_BIN)

$(UNIT_BIN): unit/test_q_math.c $(QCOMMON)/q_math.c $(UNITY)/unity.c | $(OUT)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# libFuzzer targets (clang only). Each fuzz_*.c defines LLVMFuzzerTestOneInput.
FUZZ_SAN := -fsanitize=fuzzer,address,undefined
fuzz: | $(OUT)
	@case "$$($(CC) --version 2>/dev/null)" in \
	  *clang*) ;; \
	  *) echo "fuzz targets require clang (set CC=clang); skipping."; exit 0;; \
	esac
	$(CC) -std=gnu99 -g -O1 -I$(QCOMMON) $(FUZZ_SAN) \
	  fuzz/fuzz_info.c support/stubs.c $(QCOMMON)/q_math.c $(QCOMMON)/q_shared.c \
	  -o $(OUT)/fuzz_info -lm
	@echo "built $(OUT)/fuzz_info — run: ./$(OUT)/fuzz_info -runs=100000 fuzz/corpus/info"

$(OUT):
	@mkdir -p $(OUT)

clean:
	rm -rf $(OUT)
