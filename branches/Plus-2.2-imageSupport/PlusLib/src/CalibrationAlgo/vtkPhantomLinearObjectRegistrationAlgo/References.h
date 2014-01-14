/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef REFERENCES_H
#define REFERENCES_H

#include "Reference.h"
#include <vector>

class References
{
public:
  References();
  ~References();

  void Reset();
  void InsertReference(const Reference& newReference);
  void RemoveReference(int index);
  const Reference& GetReference(int index);
  int GetNumberOfReferences();

  std::vector<Reference>::const_iterator ReferencesBegin() const;
  std::vector<Reference>::const_iterator ReferencesEnd() const;

  References& operator=(const References& rhs);

protected:
  std::vector<Reference> m_ReferenceList;
};

#endif