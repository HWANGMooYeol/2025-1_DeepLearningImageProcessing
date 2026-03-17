#ifndef _TU_DLIP_H		// same as "#if !define _TU_DLIP_H" (or #pragma once) 
#define _TU_DLIP_H

#include <iostream>

// =============================
// Exercise 1: Define Function
// =============================

int sum(int val1, int val2);



// ====================================
// Exercise 2: Create a Class "MyNum"
// ====================================

// Declare Constructor, function(sum, print), variable(val1, val2)
class MyNum
{
	// Add code here
public:
	MyNum(int x1, int x2);
	int sum(void);
	void print(void);
	int val1;
	int val2;
};

// ======================================================
// Exercise 3: Create two Class "MyNum" in proj_A, proj_B
// ======================================================
namespace proj_A
{
	// Add code here
	class MyNum
	{
		// Add code here
	public:
		MyNum(int x1, int x2, int x3);
		int sum(void);
		void print(void);
		int val1;
		int val2;
		int val3;
	};

}

namespace proj_B
{
	// Add code here
	class MyNum
	{
		// Add code here
	public:
		MyNum(int x1, int x2, int x3);
		int sum(void);
		void print(void);
		int val1;
		int val2;
		int val3;
	};
}

#endif // !_TU_DLIP_H