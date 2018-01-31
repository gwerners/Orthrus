

#include <iostream>
#include <string>

struct Sample
{
   std::string value;
   Sample(std::string _value):value(_value){std::cout << "Sample created" <<std::endl;}
   ~Sample(){std::cout << "Sample destroyed" <<std::endl;}
   void PrintSample()
   {
      std::cout << value << std::endl;
   }
};

Sample *s;

extern "C"
{
void init_sample(const char*vlr)
{
   s=new Sample(vlr);
}
void print_sample()
{
   s->PrintSample();
}
void remove_sample()
{
   delete s;
}

}
