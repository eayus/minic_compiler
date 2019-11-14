// MiniC program to compute the value of pi

extern float print_float(float X);
extern int print_int(int X);

float pi() {

  bool flag;
  float PI;
  int i;

  flag = true;
  PI = 3.0;
  i = 2;

  while(i < 100) {
    
    if(flag) {
      PI = PI + (4.0 / (i*(i+1)*(i+2)));
      //flag = false;
    }
    else {
      PI = PI - (4.0 / (i*(i+1)*(i+2)));
      //flag = true;
      print_int(69);
    }
    flag = !flag;
    i = i+2;
  }

  return PI;
}
