# Name der ausführbaren Datei
TARGET = eurorechner

# Name der Quelldatei
SRC = eurorechner.c mehrwert.c print.c

# Flags für den C-Compiler
CFLAGS = `pkg-config --cflags gtk+-3.0`

# Flags für den Linker
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Standard-Installationspfad
PREFIX = /usr/local

# Installationspfad für das Piktogramm im systemweiten Icon-Theme
ICON_DIR = /usr/share/icons/hicolor/48x48/apps

# Installationspfad für die Desktop-Integration in den systemweiten Apps
APPLICATIONS_DIR = /usr/share/applications

# Standardziel: Kompilieren und Linken
all: $(TARGET)

$(TARGET): $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Ziel zum Installieren des Programms und des Icons
install: all
	@echo "Installiere die ausführbare Datei..."
	sudo install -m 755 $(TARGET) $(PREFIX)/bin
	@echo "Installiere das Piktogramm..."
	sudo install -d $(ICON_DIR)
	sudo install -m 644 euro.png $(ICON_DIR)/$(TARGET).png
	@echo "Installiere Desktop-Integration..."
	sudo install -d $(APPLICATIONS_DIR)
	sudo install -m 644 eurorechner.desktop $(APPLICATIONS_DIR)/$(TARGET).desktop

# Ziel zum Entfernen des installierten Programms und des Icons
uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
	rm -f $(ICON_DIR)/$(TARGET).png
	rm -f $(APPLICATIONS_DIR)/$(TARGET).desktop

# Bereinigen von erstellten Dateien
clean:
	rm -f $(TARGET)

deb:
	dpkg-deb --build deb $(TARGET).deb

.PHONY: all install uninstall clean deb