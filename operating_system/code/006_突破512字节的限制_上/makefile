
.PHONY : all clean rebuild

#SRC := boot_v1.asm
#SRC := boot_v2.asm
SRC := boot_v3.asm
OUT := boot.bin
IMG := data.img

RM := rm -fr

all : $(OUT) $(IMG)
	dd if=$(OUT) of=$(IMG) bs=512 count=1 conv=notrunc
	@echo "Success!"

$(IMG) :
	bximage $@ -q -fd -size=1.44

$(OUT) : $(SRC)
	nasm $^ -o $@

clean :
	$(RM) $(IMG) $(OUT)

rebuild :
	@$(MAKE) clean
	@$(MAKE) all
	
