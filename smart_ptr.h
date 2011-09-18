#pragma once
#ifndef __CUTEPIG_SMART_PTR__
#define __CUTEPIG_SMART_PTR__

// uncomment
#define __DEBUG_PRINT_REFCOUNT__

#ifdef __DEBUG_PRINT_REFCOUNT__
	#include <iostream>
#endif

namespace cutepig {
	
	// thank you http://www.eetimes.com/discussion/programming-pointers/4025549/Catching-errors-early-with-compile-time-assertions
	#define cutepig_ctassert(cond, msg) \
	    typedef char msg[(cond) ? 1 : 0]
    
	// delete wrapper
	template<typename T>
	struct __delete__ {
		inline void operator()( T *ptr ) { delete ptr; }
	};
	
	// simple smart_ptr, deletor has to be object with operator()
	template<typename T, typename Delete= __delete__<T> >
	class smart_ptr {
	protected:
		typedef T * ptr_type;
		typedef const T *const_ptr_type;
		typedef smart_ptr<T, Delete> self_type;
		typedef self_type *self_ptr_type;
		
		ptr_type ptr;		// data referenced
		self_ptr_type link;	// next in chain
		self_ptr_type root;	// object that contains refcount info
		size_t count;		// refcount (active only in root)
		
	private:
		// private default constructor.. dont use
		smart_ptr() : ptr(0), link(0), root(this), count(0)
		{}
		
	public:
		
		// initial assignment, assigns ptr to referenced object,
		// roots the chain to itself and sets initial refcount to 1
		explicit smart_ptr(ptr_type _ptr) : ptr(_ptr), link(0), root(this), count(1)
		{
			#ifdef __DEBUG_PRINT_REFCOUNT__
				std::cout << "smart_ptr::refcount " << root->count << std::endl;
			#endif
		}
		
		// copy ctor
		explicit smart_ptr(const smart_ptr &other)
			: ptr(other.ptr), link(other.root->link), root(other.root), count(0)
		{
			// ptr, link and root are now assigned
			
			// insert to list after the root
			root->link = this;

			// refcount
			++root->count;
			
			#ifdef __DEBUG_PRINT_REFCOUNT__
				std::cout << "smart_ptr::refcount " << root->count << std::endl;
			#endif
		}
		
		// destructor, tougher one. remove reference -> possibly destroy
		// otherwise we might have to re-root the chain and remove this link
		~smart_ptr() {
			destruct();
		}
		
		self_type &operator=( const self_type &other ) {
			destruct();
			
			ptr = other.ptr;
			root = other.root;
			count = 0;
			// insert after root
			link = other.root->link;
			root->link = this;
			
			// recount
			++root->count;
			
			#ifdef __DEBUG_PRINT_REFCOUNT__
				std::cout << "smart_ptr::refcount " << root->count << std::endl;
			#endif
			
			return *this;
		}
		
		self_type &operator=( ptr_type _ptr ) {
			destruct();
			
			ptr = _ptr;
			root = this;
			link = 0;
			count = 1;
		}

		// accessors
		ptr_type operator->() { return ptr; }
		const_ptr_type operator->() const { return ptr; }
		
	private:
		void destruct() {
			// check for destruction
			if( --root->count == 0 ) {
				Delete()( ptr );
				return;
			}
			
			#ifdef __DEBUG_PRINT_REFCOUNT__
				std::cout << "smart_ptr::refcount " << root->count << std::endl;
			#endif
			
			// check for re-rooting
			if( root == this )
				reroot();
			else
				unlink();
		}
	
		// re-root assigns the 'root' for the whole chain to the next of 'root'
		void reroot() {
			root = root->link;
			// iterator
			self_ptr_type p = root;
			while( p ) {
				// re-root element
				p->root = root;
				p = p->link;
			}
		}
		
		// remove this object from the chain
		void unlink() {
			// find 'previous', downside of singly-linked list
			self_ptr_type p = root;
			while( p && p->link != this )
				p = p->link;
			if( p )
				p->link = link;
		}
	};
}

#endif
