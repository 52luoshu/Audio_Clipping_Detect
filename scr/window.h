#ifndef WINDOW_H
#define WINDOW_H


#include <vector.h>


namespace splab
{

	template<typename Type> Vector<Type> window(const string&, int, Type);
	template<typename Type> Vector<Type> window(const string&, int,
		Type, Type);

	template<typename Type> Vector<Type> rectangle(int, Type);
	template<typename Type> Vector<Type> bartlett(int, Type);
	template<typename Type> Vector<Type> hanning(int, Type);
	template<typename Type> Vector<Type> hamming(int, Type);
	template<typename Type> Vector<Type> blackman(int, Type);
	template<typename Type> Vector<Type> kaiser(int, Type, Type);
	template<typename Type> Vector<Type> gauss(int, Type, Type);

	template<typename Type> Type I0(Type alpha);


#include <window-impl>

}
// namespace splab


#endif
// WINDOW_H