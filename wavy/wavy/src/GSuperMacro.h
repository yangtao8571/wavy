/*
 * GSuperMacro.h
 *
 * Macros for Java-like 'super' in GObject
 *
 * Copyright (C) 2007 Amos Brocco
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#ifndef GSUPERMACRO_H_
#define GSUPERMACRO_H_
 
/*
 * Defines a static variable to hold the pointer to the parent's method M of type T
 * with signature P.
 * Example:
 * G_SUPER_METHOD(int,my_method,(int a, char* b));
 */
#define G_SUPER_METHOD(T,M,P) \
	static T (* g____super_##M) P;
 
/*
 * Hooks a method with the actual implementation
 * C is the class structure,
 * M is the method name, and
 * I is the name of the actual implementation
 * Example:
 * G_HOOK_METHOD(klass,my_method,real_my_method);
 */
#define G_HOOK_METHOD(C,M,I) \
	C -> M = I;
 
/*
 * Overrides a method while saving the parent's version in the static variable
 * C is the parent class structure,
 * M is the method name, and 
 * I the name of the actual implementation
 * Note: you need to use G_PRESERVE_SUPER_METHOD for every overridden method !
 * Example:
 * G_OVERRIDE_METHOD(klass,my_method,real_my_method);
 */
#define G_OVERRIDE_METHOD(C,M,I) \
	g____super_##M = C -> M; \
	C -> M = I;	
 
/*
 * Calls the super implementation of a method
 * M is the method name,
 * P are the parameters to be passed
 * Example: G_SUPER(my_method,(x,y,z));
 */
#define G_SUPER(M,P) \
	g____super_##M P;
 
#endif /*GSUPERMACRO_H_*/