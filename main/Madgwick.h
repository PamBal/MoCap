#include <math.h>

class Madgwick
{
private:
  float q0, q1, q2, q3;
  const float deltat = 0.0003f; //резкость
  const float gyroMeasError = 3.14159265358979f * (5.0f / 180.0f);
  const float beta = sqrt(3.0f/4.0f)* gyroMeasError;
  
public:
  Madgwick();
  void   FilterUpdate(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z);
  void getQuaternion(float &q_0, float &q_1, float &q_2, float &q_3);
};
