# Nom du projet
TARGET = main

# Fichiers
SRC = $(TARGET).c
OBJ = $(TARGET).o
ELF = $(TARGET).elf
BIN = $(TARGET).bin
HEX = $(TARGET).hex

# Outils
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Parametres configurables
F_CPU ?= 16000000UL         # Frequence par defaut : 16 MHz (?=) si pas de valeur, lui en assigner une.
MCU = atmega328p             # Exemple : Arduino Uno microcontroleur
PROGRAMMER = arduino         # Type de programmateur
PORT = /dev/ttyUSB0          # Port serie linux
BAUD = 115200                # Vitesse de communication (ex: Uno = 115200)

# Options de compilation
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
LDFLAGS = -mmcu=$(MCU)

all: hex flash

hex: $(BIN) $(HEX)

# compilation du code source
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# edition de liens (creation du ELF) pour debogage
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) $< -o $@

# generation du fichier binaire (.bin)
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# generation du fichier HEX (.hex)
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# flash du microcontrôleur
flash: $(HEX)
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$(HEX):i

clean:
	rm -rf $(OBJ) $(ELF) $(BIN) $(HEX)


show: all
	screen /dev/ttyUSB0 115200

.PHONY: all clean flash hex show
