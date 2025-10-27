# Arduino Project Makefile
# Bu makefile Arduino CLI kullanarak projeyi derler

# Arduino CLI ayarları
ARDUINO_CLI = arduino-cli
BOARD = arduino:avr:uno
PORT = COM3
SKETCH_DIR = .

# Gerekli kütüphaneler
LIBRARIES = RF24

.PHONY: all compile upload clean install-deps

# Varsayılan hedef
all: compile

# Kütüphaneleri yükle
install-deps:
	@echo "Kütüphaneler yükleniyor..."
	$(ARDUINO_CLI) lib install $(LIBRARIES)

# Projeyi derle
compile:
	@echo "Arduino projesi derleniyor..."
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(SKETCH_DIR)

# Derleme ve yükleme
upload: compile
	@echo "Arduino'ya yükleniyor..."
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(SKETCH_DIR)

# Temizle
clean:
	@echo "Geçici dosyalar temizleniyor..."
	@del /Q /S *.hex *.elf *.bin 2>nul || true

# Serial monitor
monitor:
	$(ARDUINO_CLI) monitor -p $(PORT)

# Arduino CLI yükle (gerekirse)
install-cli:
	@echo "Arduino CLI indiriliyor..."
	@powershell -Command "Invoke-WebRequest -Uri 'https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip' -OutFile 'arduino-cli.zip'"
	@powershell -Command "Expand-Archive -Path 'arduino-cli.zip' -DestinationPath '.'"