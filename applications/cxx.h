/************
*cxx.h
*************/
#ifndef APPLICATIONS_CXX_H_
#define APPLICATIONS_CXX_H_
 
class A
{
public:
    A();
 
    void setA(int value);
    int getA(void);
 
    void toString();
 
private:
    int a;
};
 
#endif /* APPLICATIONS_CXX_H_ */