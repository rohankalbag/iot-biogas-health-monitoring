// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

char Calc_CRC8(char *data, unsigned char Num)
{
  // function to perform the CRC error check for the specified sensor data to check for 
  // its reliability.
  unsigned char bit, byte, crc = 0xFF;
  for (byte = 0; byte < Num; byte++)
  {
    crc ^= (data[byte]);
    for (bit = 8; bit > 0; --bit)
    {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x31;
      else
        crc = (crc << 1);
    }
  }
  return crc;
}