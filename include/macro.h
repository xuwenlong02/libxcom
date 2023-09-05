#ifndef SRC_COMPONENTS_INCLUDE_UTILS_MACRO_H_
#define SRC_COMPONENTS_INCLUDE_UTILS_MACRO_H_

#include <assert.h>
#ifdef DEBUG
#include <assert.h>
#else  // RELEASE
#include <stdio.h>
#endif

// A macro to set some action for variable to avoid "unused variable" warning
#define UNUSED(x) (void) x;
// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// A macro to allow utils::Singleton call derivative constructor and destructor
#define FRIEND_BASE_SINGLETON_CLASS(TypeName) \
  friend class utils::Singleton<TypeName>

#define FRIEND_BASE_SINGLETON_CLASS_WITH_DELETER(TypeName, TypeDeleter) \
  friend class utils::Singleton<TypeName, TypeDeleter>

// A macro to allow utils::deleters::Deleter::~Deleter() call class destructor
#define FRIEND_DELETER_DESTRUCTOR(TypeName) \
  friend utils::deleters::Deleter<TypeName>::~Deleter()

#ifdef DEBUG
#define ASSERT(condition) \
  FLUSH_LOGGER();         \
  do {                    \
    DEINIT_LOGGER();      \
    assert(condition);    \
  } while (false)
#else  // RELEASE
#define ASSERT(condition) {                                      \
    if (!(condition))  {                                           \
  fprintf(stderr,                                                \
          "Failed condition \"" #condition "\" [%s:%d][%s]\n\n", \
          __FILE__,                                              \
          __LINE__,                                              \
          __FUNCTION__);                                         \
    }                                         \
    assert((condition));\
    }while(0)
#endif

#define DCHECK(condition)                                                     \
  if (!(condition)) {                                                         \
    ASSERT((condition));                                                      \
  }

/*
 * Will cauch assert on debug version,
 * Will return return_value in release build
 */
#define DCHECK_OR_RETURN(condition, return_value)                             \
  if (!(condition)) {                                                         \
    ASSERT((condition));                                                      \
    return (return_value);                                                    \
  }
/*
 * Will cauch assert on debug version,
 * Will return return_value in release build
 */
#define DCHECK_OR_RETURN_VOID(condition)                                      \
  if (!(condition)) {                                                         \
    ASSERT((condition));                                                      \
    return;                                                                   \
  }

#define EXPORT_FUNCTION(TypeName) extern "C" TypeName* Create();

#define EXPORT_FUNCTION_IMPL(TypeName) \
  extern "C" TypeName* Create() {      \
    return new TypeName();             \
  }

#define NOTREACHED() DCHECK(!"Unreachable code")

// Allows to perform static check that virtual function from base class is
// actually being overriden if compiler support is available
#if __cplusplus >= 201103L
#define OVERRIDE override
#define FINAL final
#else
#define OVERRIDE
#define FINAL
#endif

/*
* @brief Calculate size of na array
* @param arr  array, which size need to calculate
*/
#define ARRAYSIZE(arr) sizeof(arr) / sizeof(*arr)

#ifdef BUILD_TESTS
#define FRIEND_TEST(test_case_name, test_name) \
  friend class test_case_name##_##test_name##_Test
#else  // BUILD_TESTS
#define FRIEND_TEST(test_case_name, test_name)
#endif  // BUILD_TESTS

#endif  // SRC_COMPONENTS_INCLUDE_UTILS_MACRO_H_
