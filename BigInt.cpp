#include "BigInt.h"
#include <fstream>
using std::ifstream;
using std::ofstream;

/** Misc **/
inline bool isDigit(int c)
{
	return(c >= (int)'0' && c <= (int)'9');
}

/** Input/Output **/
istream &operator>>(istream &in, BigInt &n)           // FIXME: see inside
{
	n.size = 0;
	n.sign = 1;
	int sign = 1;
	int c;
	while ((c = in.peek()) >= 0 &&
		(c == ' ' || c == '\t' || c == '\r' || c == '\n'))
		in.get();
	if (c < 0 || (c != (int)'-' && !isDigit(c)))
	{
		in >> c;                // XXX: force in.fail()
		return in;
	}
	if (c == (int)'-') { sign = -1; in.get(); }

	// FIXME: Extremely inefficient! Use a string.
	while ((c = in.peek()) >= 0 && isDigit(c))
	{
		in.get();
		n = n * 10;
		n = n + (c - (int)'0');
	}
	n.sign = sign;      //XXX: assign n.sign directly after fixing the FIXME
	n.normalize();
	return in;
}

ostream &operator<<(ostream &out, BigInt n)       //FIXME: make more efficient
{
	return out << n.toString();
}

BigInt::BigInt(int n, int cap)
{
	cap = mymax(cap, (int)sizeof(n)* 8);
	capacity = cap;
	sign = sig(n);
	n *= sign;
	digits = new char[cap];
	memset(digits, 0, cap);
	for (size = 0; n; size++)
	{
		digits[size] = n % 10;
		n /= 10;
	}
}

BigInt::BigInt(int n)
{
	capacity = 1024;
	sign = sig(n);
	n *= sign;
	digits = new char[capacity];
	memset(digits, 0, capacity);
	size = 0;
	while (n)
	{
		digits[size++] = n % 10;
		n /= 10;
	}
}

BigInt::BigInt()
{
	capacity = 128;
	sign = 0;
	digits = new char[capacity];
	memset(digits, 0, capacity);
	size = 0;
}

BigInt::BigInt(const char s[])
{
	capacity = mymax((int)strlen(s), 16);
	sign = 0;
	digits = new char[capacity];
	memset(digits, 0, capacity);

	istringstream in(s);
	in >> (*this);
}

BigInt::BigInt(const BigInt &n)
{
	capacity = n.capacity;
	sign = n.sign;
	size = n.size;
	digits = new char[capacity];
	memcpy(digits, n.digits, capacity);
}

const BigInt &BigInt::operator=(const BigInt &n)
{
	if (&n != this)
	{
		if (capacity < n.size)
		{
			capacity = n.capacity;
			delete[] digits;
			digits = new char[capacity];
		}
		sign = n.sign;
		size = n.size;
		memcpy(digits, n.digits, size);
		memset(digits + size, 0, capacity - size);
	}
	return *this;
}

BigInt::~BigInt()
{
	delete[] digits;
}

void BigInt::normalize()
{
	while (size && !digits[size - 1]) size--;
	if (!size) sign = 0;
}

int BigInt::sig(int n)
{
	return(n > 0 ? 1 : (n == 0 ? 0 : -1));
}

string BigInt::toString()
{
	string s = (sign >= 0 ? "" : "-");
	for (int i = size - 1; i >= 0; i--)
		s += (digits[i] + '0');
	if (size == 0) s += '0';
	return s;
}

void BigInt::grow()
{
	char *olddigits = digits;
	int oldCap = capacity;
	capacity *= 2;
	digits = new char[capacity];
	memcpy(digits, olddigits, oldCap);
	memset(digits + oldCap, 0, oldCap);
	delete[] olddigits;
}

BigInt BigInt::operator-()
{
	BigInt result(*this);
	result.sign *= -1;
	return result;
}

BigInt BigInt::operator+(BigInt n)
{
	int maxS = mymax(size, n.size) + 1;
	BigInt res = *this;
	while (maxS >= res.capacity) res.grow();        //FIXME: this is stupid

	if (!n.sign) 
		return *this;

	if (!res.sign) 
		res.sign = n.sign;

	if (res.sign == n.sign)
	{
		int carry = 0;
		int i;
		for (i = 0; i < maxS - 1 || carry; i++)
		{
			int newdig = carry;
			if (i < size) 
				newdig += digits[i];
			if (i < n.size)
				newdig += n.digits[i];
			res.digits[i] = newdig % 10;
			carry = newdig / 10;
		}
		res.size = mymax(i, size);
		return res;
	}
	else
	{
		n.sign *= -1;
		operator-(n);
		n.sign *= -1;
	}
	return *this;
}

BigInt BigInt::operator-(BigInt n)
{
	int maxS = mymax(size, n.size) + 1;
	BigInt res = *this;
	while (maxS >= res.capacity) res.grow();        //FIXME: this is stupid

	if (!n.sign) 
		return *this;
	
	if (!this->sign)
	{
		return n;
	}

	if (res.sign == n.sign)
	{
		if (res.sign >= 0 && res < n || res.sign < 0 && res > n)
		{
			// ¬ычитание большего числа
			BigInt tmp = n;
			tmp = tmp - res;
			res = tmp;
			res.sign = -res.sign;
			return res;
		}

		int carry = 0;
		int i;
		for (i = 0; i < maxS - 1; i++)
		{
			int newdig = carry;

			if (i < res.size) 
				newdig += res.digits[i];

			if (i < n.size) 
				newdig -= n.digits[i];

			if (newdig < 0)
			{
				newdig += 10;
				carry = -1;
			}
			else carry = 0;
			res.digits[i] = newdig;
		}
		if (carry)     // Subtracted a bigger number, need to flip sign
		{// если есть заем
			if (i) 
				res.digits[0] = 10 - res.digits[0];
			size = (i ? 1 : 0);
			for (int j = 1; j < i; j++)
			{
				res.digits[j] = 9 - res.digits[j];
				if (res.digits[i]) size = j + 1;
			}
			sign *= -1;
		}
		res.normalize();
		return res;
	}
	else
	{
		n.sign *= -1;
		BigInt res = *this + n;
		n.sign *= -1;
		return res;
	}
}

BigInt BigInt::operator*(int n)
{
	BigInt result(0, size + (int)sizeof(n)* 8);
	int nsign = sig(n);
	n *= nsign;
	result.sign = sign * nsign;
	if (!result.sign) return result;

	int i, j;
	for (i = 0; n; i++)
	{
		int dig = n % 10;
		if (dig)
		{
			int carry = 0;
			for (j = 0; j < size || carry; j++)
			{
				int newDig = result.digits[i + j] + (j < size ? dig * digits[j] : 0) + carry;
				result.digits[i + j] = newDig % 10;
				carry = newDig / 10;
			}
		}
		n /= 10;
	}
	result.size = i + j - 1;
	return result;
}

BigInt BigInt::operator*(BigInt n)
{
	BigInt result(0, size + n.size);

	result.sign = sign * n.sign;
	if (!result.sign) return result;

	int i, j;
	for (i = 0; i < n.size; i++)
	{
		if (n.digits[i])
		{
			int carry = 0;
			for (j = 0; j < size || carry; j++)
			{
				int newDig =
					result.digits[i + j] +
					(j < size ? n.digits[i] * digits[j] : 0) +
					carry;
				result.digits[i + j] = newDig % 10;
				carry = newDig / 10;
			}
		}
	}
	result.size = i + j - 1;

	return result;
}

BigInt BigInt::operator/(int n)
{
	if (!n) n /= n;        //XXX: force a crash

	BigInt result(*this);
	result /= n;
	return result;
}

BigInt BigInt::operator/(BigInt n)
{
	if (!(n == 0)) n.size /= n.size;       //XXX: force a crash

	BigInt result(*this);
	result /= n;
	return result;
}

void BigInt::operator/=(int n)
{
	divide(n);
}

void BigInt::operator/=(BigInt n)
{
	divide(n);
}

BigInt BigInt::operator%(BigInt n)
{
	BigInt tmp(*this);
	return tmp.divide(n);
}

BigInt BigInt::divide(BigInt n)
{
	if (!(n == 0)) n.size /= n.size;         //XXX: force a crash

	if (!sign) return 0;
	sign *= n.sign;

	int oldSign = n.sign;
	n.sign = 1;

	BigInt tmp(0, size);
	for (int i = size - 1; i >= 0; i--)
	{
		tmp = tmp * 10;
		tmp = tmp + (int) digits[i];
		digits[i] = 0;
		while (tmp >= n) 
		{ 
			tmp = tmp - n; 
			digits[i]++; 
		}
	}
	normalize();

	n.sign = oldSign;

	return tmp;
}

bool BigInt::operator<(BigInt n)
{
	return(compare(n) < 0);
}

bool BigInt::operator>(BigInt n)
{
	return(compare(n) > 0);
}

bool BigInt::operator==(BigInt n)
{
	return(compare(n) == 0);
}

bool BigInt::operator<=(BigInt n)
{
	return(compare(n) <= 0);
}

bool BigInt::operator>=(BigInt n)
{
	return(compare(n) >= 0);
}

bool BigInt::operator<(int n)
{
	return(compare(BigInt(n)) < 0);
}

bool BigInt::operator>(int n)
{
	return(compare(BigInt(n)) > 0);
}

bool BigInt::operator==(int n)
{
	return(compare(BigInt(n)) == 0);
}

bool BigInt::operator<=(int n)
{
	return(compare(BigInt(n)) <= 0);
}

bool BigInt::operator>=(int n)
{
	return(compare(BigInt(n)) >= 0);
}

int BigInt::compare(BigInt n)
{
	if (sign < n.sign) return -1;
	if (sign > n.sign) return 1;
	if (size < n.size) return -sign;
	if (size > n.size) return sign;
	for (int i = size - 1; i >= 0; i--)
	{
		if (digits[i] < n.digits[i]) return -sign;
		else if (digits[i] > n.digits[i]) return sign;
	}
	return 0;
}

bool  BigInt::getFrom_txt(const char* filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return false;
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char * str = new char[fileSize + 1]();
	for (int i = 0; i < fileSize; i++)
	{
		str[i] = fgetc(fp);
	}

	fclose(fp);
	BigInt number(str);
	*this = number;

	delete[]str;

	return true;

}

bool BigInt::saveTo_txt(const char* filename)
{
	FILE *fp = fopen(filename, "w");
	if (!fp)
		return false;

	string str = this->toString();
	int len = str.length();
	char* string = new char[len + 1];
	fwrite(str.c_str(), sizeof(char), len, fp);
	fclose(fp);

	return true;

}

