all:
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

run: all
	qemu-system-x86_64 -drive format=raw,file=./bin/boot.bin

clean:
	rm -rf ./bin/boot.bin