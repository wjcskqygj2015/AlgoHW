#include<iostream>
using namespace std;


template<char ... args> struct ArrayHolder
{
	static const char data[sizeof ... (args)];
};

template<char ... args>
const char ArrayHolder<args ...>::data[sizeof ... (args)] ={ args ... };

template<size_t N, template<size_t> class F, char ... args>
struct generate_array_impl
{
	typedef typename generate_array_impl<N-1,F,F<N>::value,args...>::result result;
};

template<template<size_t> class F, char ... args>
struct generate_array_impl<0, F, args ...>
{
	typedef ArrayHolder<F<0>::value, args...> result;
};

template<size_t N, template<size_t> class F>
struct generate_array
{
	typedef typename generate_array_impl<N-1,F>::result result;
};


constexpr char player_sign_map[]={'X','O','L','G','H','T','S','V','M'}; 

template<size_t index>
struct SignGenerator
{
	static const char value=player_sign_map[index];
};

template<size_t index>
struct A
{
	typedef typename generate_array<index,SignGenerator>::result result;
	static constexpr const char (&markers)[index] = result::data;
};

//template<size_t index>
//constexpr char A<index>::markers[index]=generate_array<index,SignGenerator>::result::data;
int main()
{
	constexpr size_t count=5;
	for(int i=0;i<count;i++)
		cout<<A<count>::markers[i];
}
