#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}



// there is a issue in my platfrom.io when i try to create a new project it take too much long time,i wait around 5-6 min but then i clicked the cross button then i open the project it 