/*============================================================================
                    Copyright (c) 2004 MPB Technologies Inc.
                              All Rights Reserved

      This is unpublished proprietary source code of MPB Technologies Inc.
            No part of this software may be disclosed or published 
          without the expressed written consent of MPB Technologies Inc.
==============================================================================
    This file defines a C++ wrapper class for the Freedom6S API defined
  in "f6s.h".  Code for earlier versions of the library should compile
  correctly using this class.  It is encouraged that newer software avoids
  the use of functions marked "obsolete" at the bottom of the class's
  public section.
============================================================================*/

#ifndef FREEDOM6S_H
#define FREEDOM6S_H

#include "f6s.h"
#include "vector.h"
#include "matrix.h"

//////////////////
// Freedom6S Class


/*******************************************************************************
|  Some Definitions
|    Base Frame:  Fixed frame, attached to the base. X to right, Y to back, Z to top.
|    Handle Frame: Moving frame attached to the handle. X to right, Y to back, Z to top.
|    Home Frame: Fixed frame, parallel to Base Frame. Its origin is located at the 
|          Control Point when all joint angles are zero.
|    Home Position: Device position when all joint angles are zero. Base angles may be non-zero.
|    Control Point: This is the point around which torque is delivered to the handle. 
|             It is defined by the user (see setHoldDist function).
*******************************************************************************/

class Freedom6S
{
public:
    Freedom6S()
    { m_hf6s=0; m_nError = F6SRC_NOERROR; m_pErrorHandler=0; }

    ~Freedom6S()
  {
    if (m_hf6s)
      Cleanup();
  }

  bool Initialize()
  {
    m_nError = f6s_Initialize(&m_hf6s);
    Err();
    return (m_nError==F6SRC_NOERROR) && (m_hf6s!=0);
  }

  void ComputeJointVel( float ftimeStep,  int inewBufferSize = 15 )
    { m_nError = f6s_ComputeJointVel(m_hf6s, ftimeStep, inewBufferSize); Err(); }

  void SetHoldDist( float fdist )
    { m_nError = f6s_SetHoldDist( m_hf6s, fdist ); Err(); }

  void SetBaseAngles( const Vector3& knobAngVect3 )
    { m_nError = f6s_SetBaseAngles(m_hf6s, knobAngVect3.getData()); Err(); }

  void SetJointAngles( const Vector7& jointAngles )
    { m_nError = f6s_SetJointAngles(m_hf6s, jointAngles.getData(), 7); Err(); }

  void UpdateKinematics()
    { m_nError = f6s_UpdateKinematics(m_hf6s); Err(); }

  const Matrix4x4& GetPositionMatrix(void)
  {
    m_nError = f6s_GetPositionMatrix(m_hf6s, m_kineMat.getData());
    Err();
    return m_kineMat;
  }

  void GetPositionMatrixGL(double kineMat[16])
    { m_nError = f6s_GetPositionMatrixGL(m_hf6s, kineMat); Err(); }

#ifdef _D3D_H_
  void GetPositionMatrixD3D(D3DMATRIX &kineMat)
    { m_nError = f6s_GetPositionMatrixD3D(m_hf6s, &kineMat); Err(); }

  void GetPositionVectorsD3D(D3DVECTOR &pos, D3DVECTOR &dir, D3DVECTOR &up)
    { m_nError = f6s_GetPositionVectorsD3D(m_hf6s, &pos, &dir, &up); Err(); }
#endif
  
  void GetVelocity(  Vector3& vel3,  Vector3& angVel3)
    { m_nError = f6s_GetVelocity( m_hf6s, vel3.getData(), angVel3.getData() ); Err(); }

  void GetVelocityGL(  Vector3& vel3,  Vector3& angVel3)
    { m_nError = f6s_GetVelocityGL( m_hf6s, vel3.getData(), angVel3.getData() ); Err(); }

#ifdef _D3D_H_
  void GetVelocityD3D(  LPD3DVECTOR vel3,  LPD3DVECTOR angVel3)
    { m_nError = f6s_GetVelocityD3D( m_hf6s, vel3, angVel3 ); Err(); }
#endif

  void SetForceTorque(Vector3& force3, Vector3& torque3)
    { m_nError = f6s_SetForceTorque(m_hf6s, force3.getData(), torque3.getData()); Err(); }

  void SetForceTorqueGL(Vector3& force3, Vector3& torque3)
    { m_nError = f6s_SetForceTorqueGL(m_hf6s, force3.getData(), torque3.getData()); Err(); }

#ifdef _D3D_H_
  void SetForceTorqueD3D(LPD3DVECTOR force, LPD3DVECTOR torque)
    { m_nError = f6s_SetForceTorqueD3D(m_hf6s, force, torque); Err(); }
#endif

  void GetForceTorque( Vector3& force3, Vector3& torque3 )
    { m_nError = f6s_GetForceTorque(m_hf6s, force3.getData(), torque3.getData()); Err(); }

    void SetJointTorques( float jointTorques[7] )
  {
    double jointTorquesd[7];
    int i;
    for (i=0; i<7; i++)
      jointTorquesd[i] = jointTorques[i];
    m_nError = f6s_SetJointTorques( m_hf6s, jointTorquesd, 6 );
    Err();
  }

    void SetJointTorques( double jointTorques[7] )
    { m_nError = f6s_SetJointTorques( m_hf6s, jointTorques, 6 ); }

    double const* GetJointTorques( )
  {
    m_nError = f6s_GetJointTorques( m_hf6s, m_pdResult, 6 );
    Err();
    return m_pdResult;
  }

    double const* GetJointAngles()
  {
    m_nError = f6s_GetJointAngles( m_hf6s, m_pdResult, 6 );
    Err();
    return m_pdResult;
  }

    double const* GetJointVelocities()
  {
    m_nError = f6s_GetJointVelocities( m_hf6s, m_pdResult, 6 );
    Err();
    return m_pdResult;
  }

  Vector3  TransformPointToHome( const Vector3& point )
  {
    Vector3 homepoint;
    m_nError = f6s_TransformPointToHome( m_hf6s, homepoint.getData(), point.getData() );
    Err();
    return homepoint;
  }

  Vector3  TransformDirectionToHome( const Vector3& dir )
  {
    Vector3 homedir;
    m_nError = f6s_TransformDirectionToHome( m_hf6s, homedir.getData(), dir.getData() );
    Err();
    return homedir;
  }

  Vector3  TransformPointToHandle( const Vector3& point )
  {
    Vector3 handlepoint;
    m_nError = f6s_TransformPointToHandle( m_hf6s, handlepoint.getData(), point.getData() );
    Err();
    return handlepoint;
  }

  Vector3  TransformDirectionToHandle( const Vector3& dir )
  {
    Vector3 handledir;
    m_nError = f6s_TransformDirectionToHandle( m_hf6s, handledir.getData(), dir.getData() );
    Err();
    return handledir;
  }

  Vector3 GetPointVelocityHandle( const Vector3& point )
  {
    Vector3 velocity;
    m_nError = f6s_GetPointVelocityHandle( m_hf6s, velocity.getData(), point.getData() );
    Err();
    return velocity;
  }

    bool GetButtonPressed()
  {
    int pressed;
    m_nError=f6s_GetButtonPressed(m_hf6s, &pressed);
    Err();
    return pressed!=0;
  }

    void Cleanup()
  {
    if (m_hf6s) m_nError = f6s_Cleanup(m_hf6s);
    m_hf6s = 0;
    Err();
  }

    void GetDiagData( F6sDiagData* pDiag )
    { m_nError = f6s_GetDiagData(m_hf6s, pDiag, sizeof(F6sDiagData)); Err(); }

  void GetVersion( char* strVersion, int nVersionLen, char* strDate, int nDateLen )
    { m_nError = f6s_GetVersion( m_hf6s, strVersion, nVersionLen, strDate, nDateLen ); Err(); }

  void GetDeviceInfo( F6sDeviceInfo** ppDevInfo )
    { m_nError = f6s_GetDeviceInfo(m_hf6s, &m_devInfo, sizeof(F6sDeviceInfo)); *ppDevInfo = &m_devInfo; Err(); }
    
  void SetMotorVoltages( double motors[8] )
    { m_nError = f6s_SetMotorVoltages(m_hf6s, motors, 8); Err(); }

  void SetMotorVoltages( float motors[8] )
  {
    double motorsd[8];
    int i;
    for (i=0; i<8; i++)
      motorsd[i] = motors[i];
    m_nError = f6s_SetMotorVoltages(m_hf6s, motorsd, 8);
    Err();
  }

    int GetPowerStatus( )
  {
    int status;
    m_nError = f6s_GetPowerStatus(m_hf6s, &status);
    Err();
    return status;
  }

  void SetTimer( int usec )
    { f6s_SetTimer( m_hf6s, usec ); Err(); }

  void SleepTimer()
    { f6s_SleepTimer(m_hf6s); Err(); }

  void KillTimer()
    { f6s_KillTimer(m_hf6s); Err(); }

  // access error codes
  F6SRC GetLastError() { return m_nError; }
  char* GetLastErrorString()
  {
    if (f6s_GetLastErrorString(m_hf6s, m_strError, 1024) > 0)
      return m_strError;
    else
      return 0;
  }

  // obsolete, kept for backwards compatibility only
    void StandardInitialization( APPERRORHANDLER *pError = 0 )
    { m_pErrorHandler = pError; Initialize(); }
    void SetDiagData( bool bWrite_IOData ) {}
  const Matrix4x4& GetForwardKinHome(void)
    { m_nError = f6s_UpdateKinematics(m_hf6s); Err(); return GetPositionMatrix(); }

private:
  void Err() { if (GetLastErrorString() && m_pErrorHandler) m_pErrorHandler(F6SET_WARNING,m_strError); }
  HF6S m_hf6s;
  double m_pdResult[8];
  Matrix4x4 m_kineMat;
  F6SRC m_nError;
  char m_strError[1024];
  APPERRORHANDLER *m_pErrorHandler;
  F6sDeviceInfo m_devInfo;
};

#endif // FREEDOM6S_H
