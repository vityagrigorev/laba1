#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BigInt.h"
using namespace std;

void simpleMod()
{
	//todo
}
void PrintStart()
{
	printf("Program TCHMK.\n");
	printf("Enter the input parameters:\n");
	printf("\n<name of program> <filename A> <operation>\n<filename B> <result filename C> [b] [filename with module]\n\n");
	printf("Parametr: \n");
	printf("\"-b\" - binfile\n");
	printf("Operations: \n");
	printf("\"+\" - addition\n");
	printf("\"-\" - subtraction\n");
	printf("\"*\" - multiplication\n");
	printf("\"/\" - division\n");
	printf("\"%%\" - taking the remainder\n");
	printf("\"^\" - involution (pow)\n");
}
bool checkParam(int argc, char* argv[])
{
	if (argc < 2)
		//simpleMod();
		return false;

	if (argc > 7)
	{
		printf("\nToo many parameters.\n");
		PrintStart();
		return false;
	}

	if ((argc > 1) && (argc < 5))
	{
		printf("\nIntroduced not all parameters.\n");
		PrintStart();
		return false;
	}

	if (strlen(argv[2]) > 1)
	{
		printf("Invalid input operation.\n");
		PrintStart();
		return false;
	}

	return true;
}
bool getFromFiles(char* fileA, char* fileB, char* fileMod, char operation, bool bin, BigInt& a, BigInt& b, BigInt& mod)
{
	if (bin)
	{
		cout << "No bin!" << endl;
	}
	else
	{
		if (!a.getFrom_txt(fileA))
		{
			cout << "Can't get number from " << fileA << endl;
			return false;
		}
		if (!b.getFrom_txt(fileB))
		{
			cout << "Can't get number from " << fileB << endl;
			return false;
		}
		if (fileMod)
		{
			if (!mod.getFrom_txt(fileMod))
			{
				cout << "Can't get number from " << fileMod << endl;
				return false;
			}
		}
	}
	return true;
}
bool actions(BigInt& a, char operation, BigInt& b, BigInt& c, BigInt mod)
{
	if (mod < 0)
	{
		cout << "Negative module" << endl;
		return false;
	}

	else if (mod > 0 && operation != '^')
	{
		a = a % mod;
		b = b % mod;
	}

	if (operation == '+')
	{
		c = a + b;
		return true;
	}

	else if (operation == '-')
	{
		c = a - b;
		return true;
	}

	else if (operation == '*')
	{
		c = a * b;
		return true;
	}

	else if (operation == '/')
	{
		if (b == 0)
		{
			cout << "Division by zero" << endl;
			return false;
		}
		c = a / b;
		return true;
	}

	else if (operation == '%')
	{
		if (b == 0)
		{
			cout << "Division by zero" << endl;
			return false;
		}
		c = a % b;
		return true;
	}

	else if (operation == '^')
	{
		cout << "No pow!" << endl;
	}

}
bool saveFileOut(char* fileC, BigInt c, bool bin)
{
	if (bin)
	{
			cout << "NO bin!" << endl;
	}
	else
	{
		if (!c.saveTo_txt(fileC))
		{
			cout << "Can't save number to " << fileC << endl;
			return false;
		}

	}
	return true;
}

int main(int argc, char* argv[])
{
	cout << "Program TCHMK." << endl;

	if (!checkParam(argc, argv))
		return 0;

	char *fname_in1 = argv[1];
	char operation = argv[2][0];
	char *fname_in2 = argv[3];
	char *fname_out = argv[4];
	bool  bin = false;
	char *fmodule = NULL;

	if (argc == 6)
	{
		if (!strcmp(argv[5], "-b"))
			bin = true;
	}

	if (argc == 7)
		fmodule = argv[6];

	BigInt a, b, c, mod;

	if (!getFromFiles(fname_in1, fname_in2, fmodule, operation, bin, a, b, mod))
		return 0;

	if (!actions(a, operation, b, c, mod))
		return 0;

	if (!saveFileOut(fname_out, c, bin))
		return 0;
	return 0;
}

