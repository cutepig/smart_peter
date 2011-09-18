#include <iostream>
#include <list>
#include "smart_ptr.h"

using cutepig::smart_ptr;

struct myclass {
	myclass() {
		std::cout << "myclass constructor" << std::endl;
	}
	
	~myclass() {
		std::cout << "myclass destructor" << std::endl;
	}
};

struct delete_myclass {
	void operator()( myclass *c ) { delete c; }
};

int main()
{
	typedef smart_ptr<myclass, delete_myclass> myptr;
	typedef std::list<myptr> mylist;
	
	std::cout << "creating myclass" << std::endl;
	myclass *c1 = new myclass;
	
	std::cout << "creating smart peter" << std::endl;
	myptr ptr( c1 );

	// create a nice chain of these	
	{
		std::cout << "using copy ctor" << std::endl;
		myptr ptr2( ptr );
		
		{
			std::cout << "using copy ctor" << std::endl;
			myptr ptr3( ptr2 );
			
			{
				std::cout << "using copy ctor" << std::endl;
				myptr ptr3( ptr2 );
				
				std::cout << "trying to bug this" << std::endl;
				ptr2 = ptr3;
				std::cout << "what happened?" << std::endl;
			}
		}
	}
	
	std::cout << "creating mylist" << std::endl;
	mylist list;
	
	std::cout << "populating mylist" << std::endl;
	list.push_back( ptr );
	list.push_back( ptr );
	
	std::cout << "cloning mylist" << std::endl;
	mylist list2( list );
	
	std::cout << "clearing list 1" << std::endl;
	list.clear();
	
	std::cout << "clearing list 2" << std::endl;
	list2.clear();
	
	std::cout << "trying to freak this out again" << std::endl;
	myclass *c2 = new myclass;
	ptr = c2;
	std::cout << "freaked out already?" << std::endl;
	
	return 0;
}
