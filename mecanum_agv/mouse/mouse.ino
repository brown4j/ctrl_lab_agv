#include <ps2.h>

PS2 mouse(6, 5);

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Setup");

  mouse.mouse_init();
  Serial.println("Mouse Ready");
}
void loop()
{
  char stat, x, y;
  mouse.mouse_pos(stat, x, y);


  Serial.print("\tx=");
  Serial.print(x, DEC);
  Serial.print("\ty=");
  Serial.print(y, DEC);

  delay(50);

}

