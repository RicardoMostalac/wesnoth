/*
	Copyright (C) 2003 - 2022
	by David White <dave@whitevine.net>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

/**
 *  @file
 *  General math utility functions.
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>
#include <algorithm>
#include <cassert>

template<typename T>
constexpr bool is_even(T num) { return num % 2 == 0; }

template<typename T>
constexpr bool is_odd(T num) { return !is_even(num); }

/** Guarantees portable results for division by 100; round half up, to the nearest integer. */
constexpr int div100rounded(int num) {
	return (num < 0) ? -(((-num) + 50) / 100) : (num + 50) / 100;
}

/**
 * Returns base + increment, but will not increase base above max_sum, nor
 * decrease it below min_sum.
 * (If base is already below the applicable limit, base will be returned.)
 */
constexpr int bounded_add(int base, int increment, int max_sum, int min_sum = 0)
{
	if(increment >= 0) {
		return std::min(base + increment, std::max(base, max_sum));
	} else {
		return std::max(base + increment, std::min(base, min_sum));
	}
}


/**
 * @returns: the number n in [min, min+mod ) so that (n - num) is a multiple of mod.
 */
template<typename T>
constexpr T modulo(T num, int mod, T min = 0)
{
	assert(mod > 0);
	T n = (num - min) % mod;
	if (n < 0)
		n += mod;
	//n is now in [0, mod)
	n = n + min;
	return n;
	// the following properties are easy to verify:
	//  1) For all m: modulo(num, mod, min) == modulo(num + mod*m, mod, min)
	//  2) For all 0 <= m < mod: modulo(min + m, mod, min) == min + m
}

/**
 *  round (base_damage * bonus / divisor) to the closest integer,
 *  but up or down towards base_damage
 */
constexpr int round_damage(int base_damage, int bonus, int divisor) {
	if (base_damage==0) return 0;
	const int rounding = divisor / 2 - (bonus < divisor || divisor==1 ? 0 : 1);
	return std::max<int>(1, (base_damage * bonus + rounding) / divisor);
}

template<typename Cmp>
bool in_ranges(const Cmp c, const std::vector<std::pair<Cmp, Cmp>>& ranges)
{
	return std::any_of(ranges.begin(), ranges.end(), [c](const std::pair<Cmp, Cmp>& range) {
		return range.first <= c && c <= range.second;
	});
}

/**
 * Returns the size, in bits, of an instance of type `T`, providing a
 * convenient and self-documenting name for the underlying expression:
 *
 *     sizeof(T) * std::numeric_limits<unsigned char>::digits
 *
 * @tparam T The return value is the size, in bits, of an instance of this
 * type.
 *
 * @returns the size, in bits, of an instance of type `T`.
 */
template<typename T>
constexpr std::size_t bit_width() {
	return sizeof(T) * std::numeric_limits<unsigned char>::digits;
}

/**
 * Returns the size, in bits, of `x`, providing a convenient and
 * self-documenting name for the underlying expression:
 *
 *     sizeof(x) * std::numeric_limits<unsigned char>::digits
 *
 * @tparam T The return value is the size, in bits, of the type of this object.
 *
 * @returns the size, in bits, of an instance of type `T`.
 */
template<typename T>
constexpr std::size_t bit_width(const T&) {
	return sizeof(T) * std::numeric_limits<unsigned char>::digits;
}

/**
 * Returns the quantity of `1` bits in `n` — i.e., `n`’s population count.
 *
 * Algorithm adapted from:
 * <https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan>
 *
 * This algorithm was chosen for relative simplicity, not for speed.
 *
 * @tparam N The type of `n`. This should be a fundamental integer type no
 * greater than `UINT_MAX` bits in width; if it is not, the return value is
 * undefined.
 *
 * @param n An integer upon which to operate.
 *
 * @returns the quantity of `1` bits in `n`, if `N` is a fundamental integer
 * type.
 */
template<typename N>
constexpr unsigned int count_ones(N n) {
	unsigned int r = 0;
	while (n) {
		n &= n-1;
		++r;
	}
	return r;
}

// Support functions for `count_leading_zeros`.
#if defined(__GNUC__) || defined(__clang__)
constexpr unsigned int count_leading_zeros_impl(
		unsigned char n, std::size_t w) {
	// Returns the result of the compiler built-in function, adjusted for
	// the difference between the width, in bits, of the built-in
	// function’s parameter’s type (which is `unsigned int`, at the
	// smallest) and the width, in bits, of the input to this function, as
	// specified at the call-site in `count_leading_zeros`.
	return static_cast<unsigned int>(__builtin_clz(n))
		- static_cast<unsigned int>(
			bit_width<unsigned int>() - w);
}
constexpr unsigned int count_leading_zeros_impl(
		unsigned short int n, std::size_t w) {
	return static_cast<unsigned int>(__builtin_clz(n))
		- static_cast<unsigned int>(
			bit_width<unsigned int>() - w);
}
constexpr unsigned int count_leading_zeros_impl(
		unsigned int n, std::size_t w) {
	return static_cast<unsigned int>(__builtin_clz(n))
		- static_cast<unsigned int>(
			bit_width<unsigned int>() - w);
}
constexpr unsigned int count_leading_zeros_impl(
		unsigned long int n, std::size_t w) {
	return static_cast<unsigned int>(__builtin_clzl(n))
		- static_cast<unsigned int>(
			bit_width<unsigned long int>() - w);
}
constexpr unsigned int count_leading_zeros_impl(
		unsigned long long int n, std::size_t w) {
	return static_cast<unsigned int>(__builtin_clzll(n))
		- static_cast<unsigned int>(
			bit_width<unsigned long long int>() - w);
}
constexpr unsigned int count_leading_zeros_impl(
		char n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned char>(n), w);
}
constexpr unsigned int count_leading_zeros_impl(
		signed char n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned char>(n), w);
}
constexpr unsigned int count_leading_zeros_impl(
		signed short int n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned short int>(n), w);
}
constexpr unsigned int count_leading_zeros_impl(
		signed int n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned int>(n), w);
}
constexpr unsigned int count_leading_zeros_impl(
		signed long int n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned long int>(n), w);
}
constexpr unsigned int count_leading_zeros_impl(
		signed long long int n, std::size_t w) {
	return count_leading_zeros_impl(
		static_cast<unsigned long long int>(n), w);
}
#else
template<typename N>
constexpr unsigned int count_leading_zeros_impl(N n, std::size_t w) {
	// Algorithm adapted from:
	// <http://aggregate.org/MAGIC/#Leading%20Zero%20Count>
	for (unsigned int shift = 1; shift < w; shift *= 2) {
		n |= (n >> shift);
	}
	return static_cast<unsigned int>(w) - count_ones(n);
}
#endif

/**
 * Returns the quantity of leading `0` bits in `n` — i.e., the quantity of
 * bits in `n`, minus the 1-based bit index of the most significant `1` bit in
 * `n`, or minus 0 if `n` is 0.
 *
 * @tparam N The type of `n`. This should be a fundamental integer type that
 *  (a) is not wider than `unsigned long long int` (the GCC
 *   count-leading-zeros built-in functions are defined for `unsigned int`,
 *   `unsigned long int`, and `unsigned long long int`), and
 *  (b) is no greater than `INT_MAX` bits in width (the GCC built-in functions
 *   return instances of type `int`);
 * if `N` does not satisfy these constraints, the return value is undefined.
 *
 * @param n An integer upon which to operate.
 *
 * @returns the quantity of leading `0` bits in `n`, if `N` satisfies the
 * above constraints.
 *
 * @see count_leading_ones()
 */
template<typename N>
constexpr unsigned int count_leading_zeros(N n) {
#if defined(__GNUC__) || defined(__clang__)
	// GCC’s `__builtin_clz` returns an undefined value when called with 0
	// as argument.
	// [<http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html>]
	if (n == 0) {
		// Return the quantity of zero bits in `n` rather than
		// returning that undefined value.
		return static_cast<unsigned int>(bit_width(n));
	}
#endif
	// Dispatch, on the static type of `n`, to one of the
	// `count_leading_zeros_impl` functions.
	return count_leading_zeros_impl(n, bit_width(n));
	// The second argument to `count_leading_zeros_impl` specifies the
	// width, in bits, of `n`.
	//
	// This is necessary because `n` may be widened (or, alas, shrunk),
	// and thus the information of `n`’s true width may be lost.
	//
	// At least, this *was* necessary before there were so many overloads
	// of `count_leading_zeros_impl`, but I’ve kept it anyway as an extra
	// precautionary measure, that will (I hope) be optimized out.
	//
	// To be clear, `n` would only be shrunk in cases noted above as
	// having an undefined result.
}

/**
 * Returns the quantity of leading `1` bits in `n` — i.e., the quantity of
 * bits in `n`, minus the 1-based bit index of the most significant `0` bit in
 * `n`, or minus 0 if `n` contains no `0` bits.
 *
 * @tparam N The type of `n`. This should be a fundamental integer type that
 *  (a) is not wider than `unsigned long long int`, and
 *  (b) is no greater than `INT_MAX` bits in width;
 * if `N` does not satisfy these constraints, the return value is undefined.
 *
 * @param n An integer upon which to operate.
 *
 * @returns the quantity of leading `1` bits in `n`, if `N` satisfies the
 * above constraints.
 *
 * @see count_leading_zeros()
 */
template<typename N>
constexpr unsigned int count_leading_ones(N n) {
	// Explicitly specify the type for which to instantiate
	// `count_leading_zeros` in case `~n` is of a different type.
	return count_leading_zeros<N>(~n);
}

//Probably not postable.
inline int rounded_division(int a, int b)
{
	auto res = std::div(a,b);
	return 2 * res.rem > b ? (res.quot + 1) : res.quot;
}

/**
 * Converts a double to a fixed point.
 */
constexpr int32_t floating_to_fixed_point(double n)
{
	return int32_t(n * (1 << 8));
}

/**
 * @param n1 The first number to multiply.
 * @param n2 The second number to multiply.
 * @return The unsigned result of n1 * n2, then bitshifting the result to the right.
 */
constexpr unsigned fixed_point_multiply(int32_t n1, int32_t n2)
{
	return static_cast<unsigned>((n1 * n2) >> 8);
}

/**
 * @param n1 The numerator, which gets bit shifted left.
 * @param n2 The divisor.
 * @return n1 bit shifted left then divided by n1.
 */
constexpr int32_t fixed_point_divide(int n1, int n2)
{
	return (n1 << 8) / n2;
}

/**
 * If positive, just bit shift.
 * Else, make positive, bit shift, then make negative again.
 *
 * @param n The number to bit shift right.
 * @return The result of the bit shift.
 */
constexpr int fixed_point_to_int(int32_t n)
{
	if(n > 0)
	{
		return n >> 8;
	}
	else
	{
		return -(-n >> 8);
	}
}
