/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include  "References.h"

//-----------------------------------------------------------------------------

References::References()
{
}

//-----------------------------------------------------------------------------

References::~References()
{
}

//-----------------------------------------------------------------------------

void  References::Reset()
{
  this->m_ReferenceList.clear();
}

//-----------------------------------------------------------------------------

void References::InsertReference(const Reference& newReference)
{
  Reference copy = newReference;
  this->m_ReferenceList.push_back(copy);
}

//-----------------------------------------------------------------------------

void References::RemoveReference(int index)
{
  std::vector<Reference>::iterator it = this->m_ReferenceList.begin();
  it += index;
  this->m_ReferenceList.erase(it);
}

//-----------------------------------------------------------------------------

const Reference& References::GetReference(int index)
{
  return this->m_ReferenceList.at(index);
}

//-----------------------------------------------------------------------------

int References::GetNumberOfReferences()
{
  return this->m_ReferenceList.size();
}

//-----------------------------------------------------------------------------

std::vector<Reference>::const_iterator References::ReferencesBegin() const
{
  return this->m_ReferenceList.begin();
}

//-----------------------------------------------------------------------------

std::vector<Reference>::const_iterator References::ReferencesEnd() const
{
  return this->m_ReferenceList.end();
}

//-----------------------------------------------------------------------------

References& References::operator=(const References& rhs)
{
  if (this != &rhs)
  {
    this->m_ReferenceList.clear();
    this->m_ReferenceList.resize(rhs.m_ReferenceList.size());
    std::copy(rhs.ReferencesBegin(), rhs.ReferencesEnd(), this->m_ReferenceList.begin());
  }
  return *this;
}