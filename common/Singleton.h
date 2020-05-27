#ifndef SINGLETION_H
#define SINGLETION_H

#include "Errors.h"
#include "defines.h"

#define initialiseSingleton( type ) \
	template <> type * Singleton < type > :: mSingleton = 0

#define initialiseTemplateSingleton( temp, type ) \
  template <> temp< type > * Singleton < temp< type > > :: mSingleton = 0

#define createFileSingleton( type ) \
  initialiseSingleton( type ); \
  type the##type

template < class type > 
class SERVER_DECL Singleton {
public:
	Singleton( ) {
		/// If you hit this assert, this singleton already exists -- you can't create another one!
		ASSERT( this->mSingleton == 0 );
		this->mSingleton = static_cast<type *>(this);
	}
	/// Destructor
	virtual ~Singleton( ) {
		this->mSingleton = 0;
	}

	GE_INLINE static type & getSingleton( ) { ASSERT( mSingleton ); return *mSingleton; }
	GE_INLINE static type * getSingletonPtr( ) { return mSingleton; }

protected:

	/// Singleton pointer, must be set to 0 prior to creating the object
	static type * mSingleton;
};

#endif
