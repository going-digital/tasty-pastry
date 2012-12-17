"""
This code calculates sin/cos approximation code.

It uses a minmax approximation. Method based on the Sony paper
http://www.research.scea.com/research/pdfs/RGREENfastermath_GDC02.pdf


GoingDigital 2012

"""
from numpy import *
order=5
x=arange(0.00015,1,0.0001)
y=sin(2*pi*sqrt(x))/sqrt(x)
w=sqrt(x)/sin(sqrt(x))
coeffs=polyfit(x,y,deg=order,w=w)
poly=poly1d(coeffs)
print """
float sinf(float x)
{
  x*="""+'%.6g'%(.5/pi)+""";
  x-=floor(x);
  float xx=x*x;"""
print "  float y="+'%.6g'%poly[order]+";"
for i in range(order):
    print "  y=y*xx+"+'%.6g'%poly[order-i-1]+";"
print """  return x*y;
}
float cosf(float x) {
  return sinf(x+"""+'%.6g'%(pi/2)+""");
}
"""