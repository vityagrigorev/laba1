#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

#define mymin(x,y) (x) < (y) ? (x) : (y)

#define mymax(x,y) (x)> (y) ? (x) : (y)

class BigInt
{
private:
	char *digits;
	int size;            // number of used bytes (digits)
	int capacity;        // size of digits
	int sign;            // -1, 0 or +1

public:
	/** Создание большого числа из n с емкостью cap**/
	BigInt(int n, int cap);

	/** Creates a BigInt with initial value n **/
	BigInt(int n);


	/** Creates a BigInt with value 0 **/
	BigInt();

	/** Creates a BigInt by reading the value from a C string **/
	BigInt(const char s[]);

	/** Copy constructor **/
	BigInt(const BigInt &n);

	/** Assignment operator **/
	const BigInt &operator=(const BigInt &n);

	/** Cleans up **/
	~BigInt();

	/** Removes any leading zeros and adjusts the sign **/
	void normalize();

	/** Returns the sign of n: -1, 0 or 1 **/
	static int sig(int n);

	/** Arithmetic **/
	BigInt operator-();
	BigInt operator+ (BigInt n);
	BigInt operator- (BigInt n);
	BigInt operator* (int n);
	BigInt operator* (BigInt n);
	BigInt operator/ (int n);
	BigInt operator/ (BigInt n);
	void   operator/=(int n);
	void   operator/=(BigInt n);
	BigInt operator% (BigInt n);
	BigInt divide(BigInt n);        // Divides storing quotient in *this and returning the remainder

	/** Comparison **/
	bool operator<(BigInt n);
	bool operator>(BigInt n);
	bool operator==(BigInt n);
	bool operator<=(BigInt n);
	bool operator>=(BigInt n);
	bool operator<(int n);
	bool operator>(int n);
	bool operator==(int n);
	bool operator<=(int n);
	bool operator>=(int n);
	int compare(BigInt n);

	operator string();

	/** Returns the value as a decimal string **/
	string toString();

	bool getFrom_txt(const char* filename);
	bool saveTo_txt(const char* filename);

private:
	/** Expansion **/
	void grow();

	/** I/O Friends **/
	friend istream &operator>>(istream &in, BigInt &n);
	friend ostream &operator<<(ostream &out, BigInt n);
};