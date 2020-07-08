#include "Madgwick.h"

Madgwick::Madgwick()
{
  q0 = 1.0;
  q1 = 0.0;
  q2 =  0.0;
  q3 =  0.0; 
}

void Madgwick::FilterUpdate(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z)
{
  float norm; //вектор нормали
  float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; //производная от кватерниона измерений гироскопа

  float f_1, f_2, f_3; //элементы целевой функции
  float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; //элементы якобиана целевой функции

  float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; //оцениваемая ошибка

  //высчитываем значения кватернионов для подстановки в формулу описания скорости в с.о. Земли
  // по отношению к с.о. датчика
  float halfSEq_1 = 0.5f * q0;
  float halfSEq_2 = 0.5f * q1;
  float halfSEq_3 = 0.5f * q2;
  float halfSEq_4 = 0.5f * q3;
  float twoSEq_1 = 2.0f * q0;
  float twoSEq_2 = 2.0f * q1;
  float twoSEq_3 = 2.0f * q2;

  
  //вычисляем целевую функцию и якобиан
  f_1 = twoSEq_2 * q3 - twoSEq_1 * q2 - a_x;
  f_2 = twoSEq_1 * q1 + twoSEq_3 * q3 - a_y;
  f_3 = 1.0f - twoSEq_2 * q1 - twoSEq_3 * q2 - a_z;
  J_11or24 = twoSEq_3; 
  J_12or23 = 2.0f * q3;
  J_13or22 = twoSEq_1; 
  J_14or21 = twoSEq_2;
  J_32 = 2.0f * J_14or21; 
  J_33 = 2.0f * J_11or24;

  //вычисляем градиент
  SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
  SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
  SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
  SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;

  //номрализуем градиент
  norm = sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
  SEqHatDot_1 /= norm;
  SEqHatDot_2 /= norm;
  SEqHatDot_3 /= norm;
  SEqHatDot_4 /= norm;

  // вычисляем производную кватерниона с гироскопа
  SEqDot_omega_1 = -halfSEq_2 * w_x - halfSEq_3 * w_y - halfSEq_4 * w_z;
  SEqDot_omega_2 = halfSEq_1 * w_x + halfSEq_3 * w_z - halfSEq_4 * w_y;
  SEqDot_omega_3 = halfSEq_1 * w_y - halfSEq_2 * w_z + halfSEq_4 * w_x;
  SEqDot_omega_4 = halfSEq_1 * w_z + halfSEq_2 * w_y - halfSEq_3 * w_x;

  // вычисляем с учётом ошибки и интегрируем оцениваемую производную кватернина
  q0 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
  q1 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
  q2 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
  q3 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
  
  // нормализуем
  norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 /= norm;
  q1 /= norm;
  q2 /= norm;
  q3 /= norm;
}

void Madgwick::getQuaternion(float &q_0, float &q_1, float &q_2, float &q_3)
{
  q_0 = q0;
  q_1 = q1;
  q_2 = q2;
  q_3 = q3;
}
