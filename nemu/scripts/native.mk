include $(NEMU_HOME)/scripts/git.mk
include $(NEMU_HOME)/scripts/build.mk

include $(NEMU_HOME)/tools/difftest.mk

compile_git:
	$(call git_commit, "compile")
$(BINARY): compile_git

# Some convenient rules

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += $(ARGS_DIFF)

# Command to execute NEMU
IMG ?= /home/shihy/PA/ics2021/nanos-lite/build/nanos-lite-riscv32-nemu.bin
ELF ?= --elf=/home/shihy/PA/ics2021/nanos-lite/build/nanos-lite-riscv32-nemu.elf
RAMDISK ?= --ramdisk=/home/shihy/PA/ics2021/navy-apps/build/ramdisk.img
APPNAME ?= --appname=/bin/pal
NEMU_EXEC := $(BINARY) $(ARGS) $(ELF) $(RAMDISK) $(APPNAME) $(IMG)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean-tools = $(dir $(shell find ./tools -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

.PHONY: run gdb run-env clean-tools clean-all $(clean-tools)
