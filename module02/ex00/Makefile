# === Nom du projet ===
TARGET = main

# === Fichiers ===
SRC = $(TARGET).c
OBJ = $(TARGET).o
ELF = $(TARGET).elf
BIN = $(TARGET).bin
HEX = $(TARGET).hex

# === Outils ===
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# === Paramètres configurables ===
F_CPU ?= 16000000UL         # Fréquence par défaut : 16 MHz (?=) si pas de valeur, lui en assigner une.
MCU = atmega328p             # Exemple : Arduino Uno microcontroleur
PROGRAMMER = arduino         # Type de programmateur
PORT = /dev/ttyUSB0          # Port série linux
BAUD = 115200                # Vitesse de communication (ex: Uno = 115200)

# === Options de compilation ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
LDFLAGS = -mmcu=$(MCU)

all: hex flash

hex: $(BIN) $(HEX)

# Étape 1 : compilation du code source
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Étape 2 : édition de liens (création du ELF)
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) $< -o $@

# Étape 3 : génération du fichier binaire (.bin)
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Étape 4 : génération du fichier HEX (.hex)
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# === Étape 5 : flash du microcontrôleur ===
flash: $(HEX)
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$(HEX):i

# === Nettoyage ===
clean:
	rm -rf $(OBJ) $(ELF) $(BIN) $(HEX)

show: all
	screen /dev/ttyUSB0 115200

.PHONY: all clean flash hex show
