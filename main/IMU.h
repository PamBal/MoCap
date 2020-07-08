
#ifndef __IMU_H__
#define __IMU_H__

#include "MPU6050.h"
#include "Madgwick.h"


class Quat
{
  
  
public:
  float coords[4];
  Quat()
  {
    coords[0] = 1.0;
    coords[1] = 0.0;
    coords[2]=  0.0;
    coords[3]=  0.0;  
  };

  Quat(float x, float y, float z, float w)
  {
     coords[0] = x;
     coords[1] = y;
     coords[2] = z;
     coords[3] = w;
  };
 
};


class IMU
{
    MPU6050 m_mpu;
    Madgwick filter;
    int mpu_address;
    unsigned long m_t;

public:
    IMU(int _mpu_address)
    {
        m_t = 0;
        mpu_address=_mpu_address;
    };

    bool init()
    {
        while(!m_mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G, mpu_address))
        {
            Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
            delay(500);
        }

       // m_mpu.calibrateGyro(1000);
        m_mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
        
        m_mpu.setIntFreeFallEnabled(false);  
        m_mpu.setIntZeroMotionEnabled(false);
        m_mpu.setIntMotionEnabled(false);
          
        m_mpu.setDHPFMode(MPU6050_DHPF_0_63HZ);
        m_mpu.setDLPFMode(MPU6050_DLPF_6);

        m_mpu.setMotionDetectionThreshold(8);
        m_mpu.setMotionDetectionDuration(5);

        m_mpu.setZeroMotionDetectionThreshold(4);
        m_mpu.setZeroMotionDetectionDuration(2);

        m_t = millis();
        m_mpu.calibrateGyro(500);
        return true;
    };

    Quat read()
    {
        Vector tGyro = m_mpu.readNormalizeGyro();
        Vector tAxel = m_mpu.readNormalizeAccel();

        filter.FilterUpdate(tGyro.XAxis, tGyro.YAxis, tGyro.ZAxis, tAxel.XAxis, tAxel.YAxis, tAxel.ZAxis);

        Quat q;
        filter.getQuaternion(q.coords[0], q.coords[1], q.coords[2], q.coords[3]);

        return q;
    };
};

#endif // __IMU_H__
