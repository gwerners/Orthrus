

#include <iostream>
#include <string>

# define COLOR_START        "\33["
# define COLOR_GREEN        "32;49m" /*GREEN*/
# define COLOR_END          "\33[m"

extern "C"
{
void init_sample(const char*vlr)
{
   std::cout << COLOR_START COLOR_GREEN "B" COLOR_END  << " init_sample[" << vlr<< "]" <<std::endl;
}
void print_sample()
{
   std::cout << COLOR_START COLOR_GREEN "B" COLOR_END  << " print_sample" <<std::endl;
}
void remove_sample()
{
   std::cout << COLOR_START COLOR_GREEN "B" COLOR_END  << " remove_sample" <<std::endl;
}

}
