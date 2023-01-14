#ifndef STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_IMPLEMENTATION
#endif


#include <signal.h>
#include <iostream>
#include <cstdlib>

#include <iostream>
#include <complex>
#include <vector>


#include "Application.hpp"

void kiss_main(std::vector<float>& datas, const int& samplesNb);
int stk_main(int argc, char* argv[]);


int main(int argc, char* argv[])
{
	Application application(1920, 1080);

	//stk_main(argc, argv);

	return 0;
}


