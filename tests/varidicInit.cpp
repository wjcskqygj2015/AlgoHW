#include<iostream>
#include<cmath>
using namespace std;


template<template<size_t> class Func, int ... args> struct ArrayHolder
{
	static constexpr const double data[sizeof ... (args)]={ Func<args>::output ... };
};

template<template<size_t> class Func, int ... args>
constexpr const double ArrayHolder<Func, args ...>::data[sizeof ... (args)];

template<size_t N, template<size_t> class Func, template<size_t> class F, int ... args>
struct generate_array_impl
{
	typedef typename generate_array_impl<N-1,Func,F,F<N>::value,args...>::result result;
};

template<template<size_t> class Func, template<size_t> class F, int ... args>
struct generate_array_impl<0, Func, F, args ...>
{
	typedef ArrayHolder<Func, F<0>::value, args...> result;
};

template<size_t N, template<size_t> class Func, template<size_t> class F>
struct generate_array
{
	typedef typename generate_array_impl<N-1,Func,F>::result result;
};


//constexpr int player_sign_map[]={'X','O','L','G','H','T','S','V','M'}; 

template<size_t index>
struct OriginGenerator
{
	static constexpr const int value=index;
};

template<size_t input>
struct ExprDecayGenerator
{
	static constexpr const double output=pow(0.95,input);
};
//constexpr double fun(size_t x) { return pow(0.95,x); }
template<size_t index>
struct A
{
	typedef typename generate_array<index,ExprDecayGenerator,OriginGenerator>::result result;
	static constexpr const double (&markers)[index] = result::data;
};

//template<size_t index>
//constexpr int A<index>::markers[index]=generate_array<index,SignGenerator>::result::data;
int main()
{
	constexpr size_t count=40;
	for(int i=0;i<count;i++)
		cout<<A<count>::markers[i]<<' ';
}
