/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file typetraits.h
 Header file for ??? .
 */
#ifndef __TYPETRAITS_HPP__
#define __TYPETRAITS_HPP__

/**
@namespace cpp
Generic namespace for all libraries from PlutoUtils.
*/
namespace cpp {

/**
@namespace Traits
for type traits
*/
	namespace Traits {

///////////////////////////////////////////////////////////////////////////
//	class TypeTraits
///////////////////////////////////////////////////////////////////////////

		//	defines useful typetraits for types
		template<typename T>
		class TypeTraits {
		public :
			typedef T Type;
			typedef T& RefType;
			typedef T* PointerType;

		protected :
			inline explicit TypeTraits() {}
			inline ~TypeTraits() {}

		private :
			inline explicit TypeTraits(TypeTraits const&) {}
			inline TypeTraits& operator=(TypeTraits const&) { return *this; }
		};	//	end of class TypeTraits


	};	//	end of namespace Traits

};	//	end of namespace cpp


#endif
