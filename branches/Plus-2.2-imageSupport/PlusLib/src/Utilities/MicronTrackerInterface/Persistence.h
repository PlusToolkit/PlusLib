/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#ifndef __PERSISTENCE_H__
#define  __PERSISTENCE_H__

class Persistence
{
public:
  Persistence();
  ~Persistence();
  inline int getHandle() { return m_handle; };

  // Set/Get path
  int setPath(const char* path);
  //char* getPath();
  void setSection(char* section);
  int retrieveInt(char* name, int defaultVal);
  double retrieveDouble(char* name, double defaultVal);

  void saveInt(char* name, int val);
  void saveDouble(char* name, double val);

private:
  int m_handle;
  bool ownedByMe;
};

#endif
