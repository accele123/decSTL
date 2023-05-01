#ifndef _LINT_
#define _LINT_
#include <iostream>
#include <vector>
using namespace std;
class lint;
vector<char> _plus(const vector<char>& p, const vector<char>& q, int& len);
vector<char> _sub(const vector<char>& p, const vector<char>& q, int& len);
lint mul_one(const lint& p, const char q);
bool compare(const vector<char> p, const vector<char>q);
//compare������Ҫʵ�ֵ�Ӧ��ʱ�ж��޷�����p��q��p>=q
class lint
{
private:
	vector<char> val;
	//�����������
	bool sym = 0;
	//sym��Ϊ���ű��λ��0ʱΪ����1ʱΪ��
	int len = 0;
	//��¼���ݳ���
	////////////////////////////��Ԫ����/////////////////////////
	friend ostream& operator<<(ostream& out, lint it);
	friend istream& operator>>(istream& in, lint& it);
	friend lint mul_one(const lint& p, const char q);
	////////////////////////////��Ԫ����/////////////////////////
public:
	inline lint() : sym(0), len(0) {}
	inline lint(const string &input)
	{
		int i = 0;
		int L = input.length();
		if (input[0] == '-')
		{
			sym = 1;
			++i;
		}
		else
			sym = 0;
		len = 0;
		for (i; i < L; i++)
		{
			val.push_back(input[i] - 48);
			++len;
		}
	}
	inline lint(long long num)
	{
		if (num == 0)
		{
			sym = 0;
			len = 1;
			val.push_back(0);
			return;
		}
		if (num > 0)
			sym = 0;
		else
		{
			sym = 1;
			num = -num;
		}
		len = 0;
		for (num; num > 0; num /= 10)
		{
			char k = num % 10;
			val.insert(val.begin(), k);
			len++;
		}
	}
	inline lint& operator=(const lint& p)
	{
		val = p.val;
		sym = p.sym;
		len = p.len;
		return *this;
	}
	inline int Int()const
	{
		int ret = 0;
		for (auto i = val.begin(); i != val.end(); i++)
		{
			ret *= 10;
			ret += *i;
		}
		return ret;
	}
	vector<char> get_val()const { return val; }
	int get_len()const { return len; }
	bool get_sym()const { return sym; }
	bool operator>(const lint& p)const
	{
		if (sym && !p.sym)
			return 0;
		else if(!sym && p.sym)
			return 1;
		else if (!sym && !p.sym)
		{
			if (len > p.len)
				return 1;
			else if (len < p.len)
				return 0;
			else
			{
				for (int i = 0; i < len; i++)
				{
					if (val[i] > p.val[i])
					{
						return 1;
						break;
					}	
					else if (val[i] < p.val[i])
					{
						return 0;
						break;
					}
				}
			}
		}
		else
		{
			if (len < p.len)
				return 1;
			else if (len > p.len)
				return 0;
			else
			{
				for (int i = 0; i < len; i++)
				{
					if (val[i] > p.val[i])
					{
						return 0;
						break;
					}
					else if (val[i] < p.val[i])
					{
						return 1;
						break;
					}
				}
			}
		}
		return 0;
	}
	bool operator==(const lint& p)const
	{
		if (len == p.len && sym == p.sym && val == p.val)
			return 1;
		else
			return 0;
	}
	bool operator!=(const lint& p)const
	{
		if (*this == p)
			return 0;
		else
			return 1;
	}
	bool operator<(const lint& p)const
	{
		if (*this > p)
		{
			return 0;
		}
		else if (*this == p)
		{
			return 0;
		}
		return 1;
	}
	bool operator>=(const lint& p)const
	{
		if (*this > p || *this == p)
			return 1;
		else
			return 0;
	}
	bool operator<=(const lint& p)const
	{
		if (*this > p)
			return 0;
		else
			return 1;
	}
	lint operator-()const
	{
		lint ret = *this;
		if (ret.sym)
			ret.sym = 0;
		else
			ret.sym = 1;
		return ret;
	}
	lint operator+(const lint& p)const
	{
		lint ret;
		if ((sym && p.sym) || (!sym && !p.sym))
		//ͬ������
		{
			if (!sym && !p.sym)
				ret.sym = 0;
			else
				ret.sym = 1;
			//���жϷ���
			if (len >= p.len)
				ret.val = _plus(val, p.val, ret.len);
			else
				ret.val = _plus(p.val, val, ret.len);
		}
		else
		//�������
		{
			bool mark = 2;
			if (len > p.len)
				mark = 1;
			else if (len < p.len)
				mark = 0;
			else
			{
				for (int i = 0; i < len; i++)
				{
					if (val[i] > p.val[i])
					{
						mark = 1;
						break;
					}
						
					else if (val[i] < p.val[i])
					{
						mark = 0;
						break;
					}
				}
			}
			//�Ƚϼ����뱻�����ľ���ֵ�Ĵ�С
			if (mark == 2)
			{
				ret = 0;
				return ret;
			}
			if (mark == 1)
			{
				if (!sym && p.sym)
					ret.sym = 0;
				else
					ret.sym = 1;
				ret.val = _sub(val, p.val, ret.len);
			}
			else
			{
				if (!sym && p.sym)
					ret.sym = 1;
				else
					ret.sym = 0;
				ret.val = _sub(p.val, val, ret.len);
			}
		}
		while (ret.val.size() > 1)
		{
			if (*(ret.val.begin()) == 0)
			{
				ret.val.erase(ret.val.begin());
			}
			else
				break;
		}
		if (ret.val[0] == 0)
			return (lint)0;
		ret.len = ret.val.size();
		return ret;
	}
	lint operator-(const lint& p)const
	{
		return (*this + (-p));
	}
	lint operator*(const lint& p)const
	{
		if (*this == 0 || p == 0)
			return (lint)0;
		lint ret = 0;
		auto i = p.val.begin();
		for (i; i != p.val.end() - 1; i++)
		{
			ret = ret + mul_one(*this, *i);
			ret.val.push_back(0);
		}
		ret = ret + mul_one(*this, *i);
		ret.len = ret.val.size();
		if ((sym && p.sym) || (!sym && !p.sym))
			ret.sym = 0;
		else
			ret.sym = 1;
		return ret;
	}
	lint operator/(const lint& p)const
	{
		if (p == 0)
		{
			cout << "������Ϊ0��" << endl;
			return *this;
		}
		lint ret;
		char key = 0;
		bool mark = 0;
		if ((sym && p.sym) || (!sym && !p.sym))
			ret.sym = 0;
		else
			ret.sym = 1;
		lint pass;
		pass.sym = 0;
		for (auto i = val.begin(); i != val.end(); i++)
		{
			key = 0;
			if (pass == 0)
				pass.val.clear();
			pass.val.push_back(*i);
			pass.len = pass.val.size();
			while (compare(pass.val, p.val))
			{
				pass = (pass - p);
				++key;
			}
			if (key != 0)
				mark = 1;
			if (mark)
				ret.val.push_back(key);
		}
		if (ret.val.size() == 0)
		{
			return (lint)0;
		}
		ret.len = ret.val.size();
		return ret;
	}
	lint operator%(const lint& p)const
	{
		lint ret;
		lint k = p;
		k.sym = 0;
		ret = *this - (*this / k) * k;
		return ret;
	}
	lint operator+=(const lint& p)
	{
		*this = *this + p;
		return *this;
	}
	lint operator-=(const lint& p)
	{
		*this = *this - p;
		return *this;
	}
	lint operator*=(const lint& p)
	{
		*this = *this * p;
		return *this;
	}
	lint operator/=(const lint& p)
	{
		*this = *this / p;
		return *this;
	}
	lint operator%=(const lint& p)
	{
		*this = *this % p;
		return *this;
	}
	lint operator++(int)
	{
		lint p = *this;
		*this += 1;
		return p;
	}
	lint operator++()
	{
		*this += 1;
		return *this;
	}
	lint operator--(int)
	{
		lint p = *this;
		*this -= 1;
		return p;
	}
	lint operator--()
	{
		*this -= 1;
		return *this;
	}
	lint pow(const lint& p)const
	{
		lint ret = *this;
		for (lint i = 0; i < p; ++i)
		{
			ret = ret * *this;
		}
		return ret;
	}
};
ostream& operator<<(ostream& out, lint it)
{
	auto e = it.val.end();
	if (it.sym)
		out << '-';
	for (auto i = it.val.begin(); i != e; i++)
	{
		printf("%d", *i);
	}
	return out;
}
istream& operator>>(istream& in, lint &it)
{
	string input;
	in >> input;
	int i = 0;
	int len = input.length();
	if (input[0] == '-')
	{
		it.sym = 1;
		++i;
	}
	else
	{
		it.sym = 0;
	}
	it.len = 0;
	for (i; i < len; i++)
	{
		it.val.push_back(input[i] - 48);
		++it.len;
	}
	return in;
}
bool operator>(const int k, const lint& p)
{
	if ((lint)k > p)
		return 1;
	else
		return 0;
}
bool operator>=(const int k, const lint& p)
{
	if ((lint)k >= p)
		return 1;
	else
		return 0;
}
bool operator<(const int k, const lint& p)
{
	if ((lint)k < p)
		return 1;
	else
		return 0;
}
bool operator<=(const int k, const lint& p)
{
	if ((lint)k <= p)
		return 1;
	else
		return 0;
}
bool operator==(const int k, const lint& p)
{
	if ((lint)k == p)
		return 1;
	else
		return 0;
}
bool operator!=(const int k, const lint& p)
{
	if (k == p)
		return 0;
	else
		return 1;
}
lint operator+(const int p, const lint& q)
{
	return (lint)p + q;
}
lint operator-(const int p, const lint& q)
{
	return (lint)p - q;
}
lint operator*(const int p, const lint& q)
{
	return (lint)p * q;
}
lint operator/(const int p, const lint& q)
{
	return (lint)p / q;
}
lint operator%(const int p, const lint& q)
{
	return (lint)p % q;
}
void operator+=(int p, const lint& q)
{
	p += q.Int();
}
void operator-=(int p, const lint& q)
{
	p -= q.Int();
}
void operator*=(int p, const lint& q)
{
	p *= q.Int();
}
void operator/=(int p, const lint& q)
{
	p /= q.Int();
}
vector<char> _plus(const vector<char>& p, const vector<char>& q, int& len)
{
	vector<char> ret;
	ret = p;
	auto i = ret.rbegin();
	auto j = q.rbegin();
	for (i, j; j != q.rend(); i++, j++)
	{
		*i = *i + *j;
	}
	auto k = ret.rbegin();
	for (k; k != ret.rend() - 1; k++)
	{
		if (*k >= 10)
		{
			(*k) -= 10;
			++(*(k + 1));
		}
	}
	if (*k >= 10)
	{
		(*k) -= 10;
		ret.insert(ret.begin(), 1);
	}
	len = ret.size();
	return ret;
}
vector<char> _sub(const vector<char>& p, const vector<char>& q, int& len)
{
	vector<char> ret;
	ret = p; 
	auto i = ret.rbegin();
	auto j = q.rbegin();
	for (i, j; j != q.rend(); i++, j++)
	{
		*i = *i - *j;
	}
	auto k = ret.rbegin();
	for (k; k != ret.rend() - 1; k++)
	{
		if (*k < 0)
		{
			(*k) += 10;
			--(*(k + 1));
		}
	}
	len = ret.size();
	return ret;
}
lint mul_one(const lint& p, const char q)
{
	lint ret = 0;
	ret.sym = 0;
	auto i = p.val.begin();
	for (i; i != p.val.end() - 1; i++)
	{
		ret = ret + (*i) * q;
		ret.val.push_back(0);
	}
	ret = ret + (*i) * q;
	ret.len = ret.val.size();
	return ret;
}
bool compare(const vector<char> p, const vector<char>q)
//compare������Ҫʵ�ֵ�Ӧ��ʱ�ж��޷�����p��q��p>=q
{
	if (p.size() > q.size())
		return 1;
	else if (p.size() < q.size())
		return 0;
	else
	{
		int len = p.size();
		for (int i = 0; i < len; i++)
		{
			if (p[i] > q[i])
			{
				return 1;
				break;
			}
			else if (p[i] < q[i])
			{
				return 0;
				break;
			}
		}
	}
	return 1;
}
vector<char> _bit(const lint& p)
{
	lint remainder;
	lint p1 = p;
	vector<char> temp;
	while (p1 >= 2)
	{
		remainder = p1 % 2;
		p1 /= 2;
		if (remainder == 1)
			temp.push_back(1);
		else
			temp.push_back(0);
	}
	if (p1 == 1)
		temp.push_back(1);
	vector<char> ret(temp.size());
	for (size_t i = 0; i < temp.size(); i++)
		ret[temp.size() - 1 - i] = temp[i];
	return ret;
}
#endif