/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2007 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/


#ifndef _FIXED_H
#define _FIXED_H 1 

/*!
  \class fixed
  \brief A fixed-point numeric value type, with the pivot between bit 13 and bit 14.

  All basic arithmetic operations are supported.
  For balancing, this type lies between the integer and float types.

  VERY IMPORTANT: Note that the "*" (multiply) operator doesn't check for overflow,
  and assumes that the result of the multiplication is less than 2.0,
  the "*=" operator will work for products up to 131071.0 but is much
  less efficient.

  Three special methods are provided for the sake of efficient conversion:
  f.floor()  - convert to int using round-to-negative-infinity
  f.ceil()   - convert to int using round-to-positive-infinity
  f.round()  - convert to int using round-to-nearest, round to the larger
               number in case of a tie

  \ingroup PlusLibVolumeReconstruction
*/
class fixed
{
private:

  enum ConversionType
  {
    /*! 
      A constant that is used as a second parameter in the fixed constructor
      when the input value is already encoded as fixed and so no conversion
      is necessary
    */
    INTERPRET_AS_FIXED
  };

  static inline int point() { return 14; };

  static inline int from_float(double x) {
    union { double d; unsigned int i[2]; } dual;
    dual.d = x + 412316860416.0; // (2**(52-point()))*1.5
    return dual.i[0]; }; // warning: some compilers screw this up

  static inline double to_float(int x) {
    return x*(1.0/(1<<point())); };

  static inline int to_int(int x) {
    return ((x >= 0) ? (x>>point()) : ((x + ((1<<point())-1))>>point())); };

  static inline int from_int(int x) {
    return x<<point(); };

  static inline int fast_multiply(int x, int y) {
    return ((x*y + (1<<(point()-1)))>>point()); };

  static inline int multiply(int x, int y) {
    int hx = x>>point();
    int hy = y>>point();
    int lx = x - (hx<<point());
    int ly = y - (hy<<point());
    return ((lx*ly + (1<<(point()-1)))>>point()) + hx*ly + x*hy; };
    
  static inline int divide(int x, int y) {
    return from_float(double(x)/y); };

  /*! Dummy null-conversion (to directly set the value of the integer container with an existing integer, without conversion */
  inline fixed(int x, ConversionType) : i(x) {};

public:
  /*! This member stores the fixed point number */
  int i;

  /*! Constructs the object. Its value is not initialized. */
  inline fixed() {};
  inline fixed(const fixed &x) : i(x.i) {}; 
  inline fixed& operator=(const fixed& x) { i = x.i; return *this; };

  inline fixed(long x) : i(from_int(x)) {};
  inline fixed(unsigned long x) : i(from_int(x)) {};
  inline fixed(int x) : i(from_int(x)) {};
  inline fixed(unsigned int x) : i(from_int(x)) {};
  inline fixed(short x) : i(from_int(x)) {};
  inline fixed(unsigned short x) : i(from_int(x)) {};
  inline fixed(char x) : i(from_int(x)) {};
  inline fixed(unsigned char x) : i(from_int(x)) {};
  inline fixed(float x) : i(from_float(x)) {};
  inline fixed(double x) : i(from_float(x)) {};  

  inline operator long() const { return to_int(i); };
  inline operator unsigned long() const { return to_int(i); };
  inline operator int() const { return to_int(i); };
  inline operator unsigned int() const { return to_int(i); };
  inline operator short() const { return to_int(i); };
  inline operator unsigned short() const { return to_int(i); };
  inline operator char() const { return to_int(i); };
  inline operator unsigned char() const { return to_int(i); };
  inline operator float() const { return (float)to_float(i); };
  inline operator double() const { return to_float(i); };

  inline fixed& operator+=(const fixed& x) { i += x.i; return *this; };
  inline fixed& operator-=(const fixed& x) { i -= x.i; return *this; };
  inline fixed& operator*=(const fixed& x) { 
    i = multiply(i, x.i); return *this; };
  inline fixed& operator/=(const fixed& x) {
    i = divide(i, x.i); return *this; };

  inline fixed& operator*=(long x) { i *= x; return *this; };
  inline fixed& operator*=(unsigned long x) { i *= x; return *this; };
  inline fixed& operator*=(int x) { i *= x; return *this; };
  inline fixed& operator*=(unsigned int x) { i *= x; return *this; };
  inline fixed& operator*=(short x) { i *= x; return *this; };
  inline fixed& operator*=(unsigned short x) { i *= x; return *this; };
  inline fixed& operator*=(char x) { i *= x; return *this; };
  inline fixed& operator*=(unsigned char x) { i *= x; return *this; };
  inline fixed& operator*=(float x) {
    i = from_float(float(*this)*x); return *this; };
  inline fixed& operator*=(double x) {
    i = from_float(double(*this)*x); return *this; };  

  friend const fixed& operator+(const fixed& x);
  friend fixed operator-(const fixed& x);

  friend fixed operator+(const fixed& x, const fixed& y);
  friend fixed operator-(const fixed& x, const fixed& y);
  friend fixed operator*(const fixed& x, const fixed& y);
  friend fixed operator/(const fixed& x, const fixed& y);

  friend fixed operator*(int x, const fixed& y);
  friend fixed operator*(const fixed& x, int y);

  friend bool operator==(const fixed& x, const fixed& y);
  friend bool operator!=(const fixed& x, const fixed& y);
  friend bool operator<(const fixed& x, const fixed& y);
  friend bool operator>(const fixed& x, const fixed& y);
  friend bool operator<=(const fixed& x, const fixed& y);
  friend bool operator>=(const fixed& x, const fixed& y);

  inline int round() const { return (i + (1<<(point()-1)))>>point(); };
  inline int floor() const { return i >> point(); };
  inline int ceil() const { return (i + ((1<<point())-1))>>point(); };
};

inline const fixed& operator+(const fixed& x) {
  return x; }
inline fixed operator-(const fixed& x) {
  return fixed(-x.i, fixed::INTERPRET_AS_FIXED); }

inline fixed operator+(const fixed& x, const fixed& y) {
  return fixed(x.i + y.i, fixed::INTERPRET_AS_FIXED); }
inline fixed operator-(const fixed& x, const fixed& y) {
  return fixed(x.i - y.i, fixed::INTERPRET_AS_FIXED); }
inline fixed operator*(const fixed& x, const fixed& y) {
  return fixed(fixed::fast_multiply(x.i, y.i), fixed::INTERPRET_AS_FIXED); }
inline fixed operator/(const fixed& x, const fixed& y) {
  return fixed(fixed::divide(x.i, y.i), fixed::INTERPRET_AS_FIXED); }

inline bool operator==(const fixed& x, const fixed& y) {
  return x.i == y.i; }
inline bool operator!=(const fixed& x, const fixed& y) {
  return x.i != y.i; }
inline bool operator>(const fixed& x, const fixed& y) {
  return x.i > y.i; }
inline bool operator<(const fixed& x, const fixed& y) {
  return x.i < y.i; }
inline bool operator>=(const fixed& x, const fixed& y) {
  return x.i >= y.i; }
inline bool operator<=(const fixed& x, const fixed& y) {
  return x.i <= y.i; }

inline fixed operator+(int x, const fixed& y) {
  return fixed(x) + y; }
inline fixed operator+(const fixed& x, int y) {
  return x + fixed(y); }

inline fixed operator-(int x, const fixed& y) {
  return fixed(x) - y; }
inline fixed operator-(const fixed& x, int y) {
  return x - fixed(y); }

inline fixed operator*(int x, const fixed& y) {
  return fixed(x*y.i, fixed::INTERPRET_AS_FIXED); }
inline fixed operator*(const fixed& x, int y) {
  return fixed(x.i*y, fixed::INTERPRET_AS_FIXED); }

inline fixed operator/(int x, const fixed& y) {
  return fixed(x) /= y; }
inline fixed operator/(const fixed& x, int y) {
  return x / fixed(y); }

inline bool operator==(const fixed& x, int y) { return x == fixed(y); }
inline bool operator==(int x, const fixed& y) { return fixed(x) == y; }

inline bool operator!=(const fixed& x, int y) { return x != fixed(y); }
inline bool operator!=(int x, const fixed& y) { return fixed(x) != y; }

inline bool operator<(const fixed& x, int y) { return x < fixed(y); }
inline bool operator<(int x, const fixed& y) { return fixed(x) < y; }

inline bool operator>(const fixed& x, int y) { return x > fixed(y); }
inline bool operator>(int x, const fixed& y) { return fixed(x) > y; }

inline bool operator<=(const fixed& x, int y) { return x <= fixed(y); }
inline bool operator<=(int x, const fixed& y) { return fixed(x) <= y; }

inline bool operator>=(const fixed& x, int y) { return x >= fixed(y); }
inline bool operator>=(int x, const fixed& y) { return fixed(x) >= y; }


inline bool operator==(const fixed& x, unsigned int y) {
  return x == fixed(y); }
inline bool operator==(unsigned int x, const fixed& y) {
  return fixed(x) == y; }

inline bool operator!=(const fixed& x, unsigned int y) {
  return x != fixed(y); }
inline bool operator!=(unsigned int x, const fixed& y) {
  return fixed(x) != y; }

inline bool operator<(const fixed& x, unsigned int y) {
  return x < fixed(y); }
inline bool operator<(unsigned int x, const fixed& y) {
  return fixed(x) < y; }

inline bool operator>(const fixed& x, unsigned int y) {
  return x > fixed(y); }
inline bool operator>(unsigned int x, const fixed& y) {
  return fixed(x) > y; }

inline bool operator<=(const fixed& x, unsigned int y) {
  return x <= fixed(y); }
inline bool operator<=(unsigned int x, const fixed& y) {
  return fixed(x) <= y; }

inline bool operator>=(const fixed& x, unsigned int y) {
  return x >= fixed(y); }
inline bool operator>=(unsigned int x, const fixed& y) {
  return fixed(x) >= y; }


inline bool operator!=(const fixed& x, long y) { return x != fixed(y); }
inline bool operator!=(long x, const fixed& y) { return fixed(x) != y; }

inline bool operator<(const fixed& x, long y) { return x < fixed(y); }
inline bool operator<(long x, const fixed& y) { return fixed(x) < y; }

inline bool operator>(const fixed& x, long y) { return x > fixed(y); }
inline bool operator>(long x, const fixed& y) { return fixed(x) > y; }

inline bool operator<=(const fixed& x, long y) { return x <= fixed(y); }
inline bool operator<=(long x, const fixed& y) { return fixed(x) <= y; }

inline bool operator>=(const fixed& x, long y) { return x >= fixed(y); }
inline bool operator>=(long x, const fixed& y) { return fixed(x) >= y; }


inline bool operator==(const fixed& x, unsigned long y) {
  return x == fixed(y); }
inline bool operator==(unsigned long x, const fixed& y) {
  return fixed(x) == y; }

inline bool operator!=(const fixed& x, unsigned long y) {
  return x != fixed(y); }
inline bool operator!=(unsigned long x, const fixed& y) {
  return fixed(x) != y; }

inline bool operator<(const fixed& x, unsigned long y) {
  return x < fixed(y); }
inline bool operator<(unsigned long x, const fixed& y) {
  return fixed(x) < y; }

inline bool operator>(const fixed& x, unsigned long y) {
  return x > fixed(y); }
inline bool operator>(unsigned long x, const fixed& y) {
  return fixed(x) > y; }

inline bool operator<=(const fixed& x, unsigned long y) {
  return x <= fixed(y); }
inline bool operator<=(unsigned long x, const fixed& y) {
  return fixed(x) <= y; }

inline bool operator>=(const fixed& x, unsigned long y) {
  return x >= fixed(y); }
inline bool operator>=(unsigned long x, const fixed& y) {
  return fixed(x) >= y; }


inline fixed operator+(long x, const fixed& y) { return int(x) + y; }
inline fixed operator+(unsigned long x, const fixed& y) { return int(x) + y; }
inline fixed operator+(unsigned int x, const fixed& y) { return int(x) + y; }

inline fixed operator+(const fixed& x, long y) { return x + int(y); }
inline fixed operator+(const fixed& x, unsigned long y) { return x + int(y); }
inline fixed operator+(const fixed& x, unsigned int y) { return x + int(y); }

inline fixed operator-(long x, const fixed& y) { return int(x) - y; }
inline fixed operator-(unsigned long x, const fixed& y) { return int(x) - y; }
inline fixed operator-(unsigned int x, const fixed& y) { return int(x) - y; }

inline fixed operator-(const fixed& x, long y) { return x - int(y); }
inline fixed operator-(const fixed& x, unsigned long y) { return x - int(y); }
inline fixed operator-(const fixed& x, unsigned int y) { return x - int(y); }

inline fixed operator*(long x, const fixed& y) { return int(x) * y; }
inline fixed operator*(unsigned long x, const fixed& y) { return int(x) * y; }
inline fixed operator*(unsigned int x, const fixed& y) { return int(x) * y; }

inline fixed operator*(const fixed& x, long y) { return x * int(y); }
inline fixed operator*(const fixed& x, unsigned long y) { return x * int(y); }
inline fixed operator*(const fixed& x, unsigned int y) { return x * int(y); }

inline fixed operator/(long x, const fixed& y) { return int(x) / y; }
inline fixed operator/(unsigned long x, const fixed& y) { return int(x) / y; }
inline fixed operator/(unsigned int x, const fixed& y) { return int(x) / y; }

inline fixed operator/(const fixed& x, long y) { return x / int(y); }
inline fixed operator/(const fixed& x, unsigned long y) { return x / int(y); }
inline fixed operator/(const fixed& x, unsigned int y) { return x / int(y); }

inline fixed operator+(short x, const fixed& y) { return int(x) + y; }
inline fixed operator+(unsigned short x, const fixed& y) { return int(x) + y; }
inline fixed operator+(char x, const fixed& y) { return int(x) + y; }
inline fixed operator+(unsigned char x, const fixed& y) { return int(x) + y; }

inline fixed operator+(const fixed& x, short y) { return x + int(y); }
inline fixed operator+(const fixed& x, unsigned short y) { return x + int(y); }
inline fixed operator+(const fixed& x, char y) { return x + int(y); }
inline fixed operator+(const fixed& x, unsigned char y) { return x + int(y); }

inline fixed operator-(short x, const fixed& y) { return int(x) - y; }
inline fixed operator-(unsigned short x, const fixed& y) { return int(x) - y; }
inline fixed operator-(char x, const fixed& y) { return int(x) - y; }
inline fixed operator-(unsigned char x, const fixed& y) { return int(x) - y; }

inline fixed operator-(const fixed& x, short y) { return x - int(y); }
inline fixed operator-(const fixed& x, unsigned short y) { return x - int(y); }
inline fixed operator-(const fixed& x, char y) { return x - int(y); }
inline fixed operator-(const fixed& x, unsigned char y) { return x - int(y); }

inline fixed operator*(short x, const fixed& y) { return int(x) * y; }
inline fixed operator*(unsigned short x, const fixed& y) { return int(x) * y; }
inline fixed operator*(char x, const fixed& y) { return int(x) * y; }
inline fixed operator*(unsigned char x, const fixed& y) { return int(x) * y; }

inline fixed operator*(const fixed& x, short y) { return x * int(y); }
inline fixed operator*(const fixed& x, unsigned short y) { return x * int(y); }
inline fixed operator*(const fixed& x, char y) { return x * int(y); }
inline fixed operator*(const fixed& x, unsigned char y) { return x * int(y); }

inline fixed operator/(short x, const fixed& y) { return int(x) / y; }
inline fixed operator/(unsigned short x, const fixed& y) { return int(x) / y; }
inline fixed operator/(char x, const fixed& y) { return int(x) / y; }
inline fixed operator/(unsigned char x, const fixed& y) { return int(x) / y; }

inline fixed operator/(const fixed& x, short y) { return x / int(y); }
inline fixed operator/(const fixed& x, unsigned short y) { return x / int(y); }
inline fixed operator/(const fixed& x, char y) { return x / int(y); }
inline fixed operator/(const fixed& x, unsigned char y) { return x / int(y); }

inline double operator+(double x, const fixed& y) { return x + double(y); }
inline double operator+(const fixed& x, double y) { return double(x) + y; }

inline double operator-(double x, const fixed& y) { return x - double(y); }
inline double operator-(const fixed& x, double y) { return double(x) - y; }

inline double operator*(double x, const fixed& y) { return x * double(y); }
inline double operator*(const fixed& x, double y) { return double(x) * y; }

inline double operator/(double x, const fixed& y) { return x / double(y); }
inline double operator/(const fixed& x, double y) { return double(x) / y; }

inline bool operator==(double x, const fixed& y) {return x == double(y);}
inline bool operator==(const fixed& x, double y) {return double(x) == y;}

inline bool operator!=(double x, const fixed& y) {return x != double(y);}
inline bool operator!=(const fixed& x, double y) {return double(x) != y;}

inline bool operator<(double x, const fixed& y) {return x < double(y);}
inline bool operator<(const fixed& x, double y) {return double(x) < y;}

inline bool operator>(double x, const fixed& y) {return x > double(y);}
inline bool operator>(const fixed& x, double y) {return double(x) > y;}

inline bool operator<=(double x, const fixed& y) {return x <= double(y);}
inline bool operator<=(const fixed& x, double y) {return double(x) <= y;}

inline bool operator>=(double x, const fixed& y) {return x >= double(y);}
inline bool operator>=(const fixed& x, double y) {return double(x) >= y;}

inline float operator+(float x, const fixed& y) { return x + float(y); }
inline float operator+(const fixed& x, float y) { return float(x) + y; }

inline float operator-(float x, const fixed& y) { return x - float(y); }
inline float operator-(const fixed& x, float y) { return float(x) - y; }

inline float operator*(float x, const fixed& y) { return x * float(y); }
inline float operator*(const fixed& x, float y) { return float(x) * y; }

inline float operator/(float x, const fixed& y) { return x / float(y); }
inline float operator/(const fixed& x, float y) { return float(x) / y; }

inline bool operator==(float x, const fixed& y) { return x == float(y); }
inline bool operator==(const fixed& x, float y) { return float(x) == y; }

inline bool operator!=(float x, const fixed& y) { return x != float(y); }
inline bool operator!=(const fixed& x, float y) { return float(x) != y; }

inline bool operator<(float x, const fixed& y) { return x < float(y); }
inline bool operator<(const fixed& x, float y) { return float(x) < y; }

inline bool operator>(float x, const fixed& y) { return x > float(y); }
inline bool operator>(const fixed& x, float y) { return float(x) > y; }

inline bool operator<=(float x, const fixed& y) { return x <= float(y); }
inline bool operator<=(const fixed& x, float y) { return float(x) <= y; }

inline bool operator>=(float x, const fixed& y) { return x >= float(y); }
inline bool operator>=(const fixed& x, float y) { return float(x) >= y; }

#endif // _FIXED_H
