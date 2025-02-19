#ifndef RFID_NFC_SETTINGS_H
#define RFID_NFC_SETTINGS_H
#define DISABLE_ALL_LIBRARY_WARNINGS
void setupRFID();
void rfidScan();
void rfidScanContinuous();  // New continuous scanning mode
void rfidClone();  // New cloning function
void formatRFIDCard();

#endif // RFID_NFC_SETTINGS_H
