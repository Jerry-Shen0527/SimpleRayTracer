#pragma once
#include <memory>

class BigInt
{
	BigInt();
	BigInt(const BigInt& big);
	BigInt(BigInt&& big) noexcept;

	BigInt& operator=(const BigInt& big);
	BigInt& operator=(BigInt&& big)noexcept;
	~BigInt();

	BigInt operator+(const BigInt& big) const;
	BigInt operator-(const BigInt& big) const;
	BigInt operator*(const BigInt& big) const;
	BigInt operator/(const BigInt& big) const;
	BigInt operator%(const BigInt& big) const;

private:
	unsigned size;
	std::unique_ptr<int> data;
};


