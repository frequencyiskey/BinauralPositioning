//Gabriella Watkins U1154477
#include "BinauralConvolution.h"

using namespace std;

int main()
{
	int loop = 0;

	cout << "Binaural Convolution" << endl << "Individual Project Artefact"
		<< endl << "Gabriella Watkins (U1154477)" << endl << endl;

	while (loop == 0)
	{
		BinauralConvolution execute2 = BinauralConvolution();
		loop = execute2.controlFunction();
	}
	return 0;
}
