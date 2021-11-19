// How long do we wait between two subsequent characters sent via bluetooth
#define BT_FREQUENCY 50

// Calculate the Bluetooth character to send to the device from the positon of the two axis
char getCommand(int ax1, int ax2){
  delay(50);
  if (ax1 > 400 && ax1 < 600 && ax2 > 400 && ax2 < 600) {
    return 'S';
  }
  if (ax1 > 400 && ax1 < 600 && ax2 > 600) {
    return 'F';    
  }
  if (ax1 > 400 && ax1 < 600 && ax2 < 400) {
    return 'B';
  }
  if (ax1 < 400  && ax2 > 400 && ax2 < 600) {
    return 'L';
  }
  if (ax1 > 600 && ax2 > 400 && ax2 < 600) {
    return 'R';
  }
  if (ax1 > 600 &&  ax2 > 600) {
    return 'I';
  }
  if (ax1 < 400 && ax2 > 600) {
    return 'G';
  }
  if (ax1 > 600 && ax2 < 400) {
    return 'J';
  }
  if (ax1 < 400  && ax2 < 400) {
    return 'H';
  }
}
