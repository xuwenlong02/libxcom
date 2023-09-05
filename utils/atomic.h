#ifndef SRC_COMPONENTS_INCLUDE_UTILS_ATOMIC_H_
#define SRC_COMPONENTS_INCLUDE_UTILS_ATOMIC_H_


#if defined(__QNXNTO__)
#define atomic_post_inc(ptr) atomic_add_value((ptr), 1)
#elif defined(__GNUG__)
#define atomic_post_inc(ptr) __sync_fetch_and_add((ptr), 1)
#else
#warning "atomic_post_inc() implementation is not atomic"
#define atomic_post_inc(ptr) (*(ptr))++
#endif

#if defined(__QNXNTO__)
#define atomic_post_dec(ptr) atomic_sub_value((ptr), 1)
#elif defined(__GNUG__)
#define atomic_post_dec(ptr) __sync_fetch_and_sub((ptr), 1)
#else
#warning "atomic_post_dec() implementation is not atomic"
#define atomic_post_dec(ptr) (*(ptr))--
#endif

#if defined(__QNXNTO__)
// on QNX pointer assignment is believed to be atomic
#define atomic_pointer_assign(dst, src) (dst) = (src)
#elif defined(__GNUG__)
// with g++ pointer assignment is believed to be atomic
#define atomic_pointer_assign(dst, src) (dst) = (src)
#else
#warning atomic_pointer_assign() implementation may be non-atomic
#define atomic_pointer_assign(dst, src) (dst) = (src)
#endif

#if defined(__QNXNTO__)
#define atomic_post_set(dst) atomic_set_value(dst, 1)
#elif defined(__GNUG__)
#define atomic_post_set(dst) __sync_val_compare_and_swap((dst), 0, 1)
#else
#error "atomic post set operation not defined"
#endif

#if defined(__QNXNTO__)
#define atomic_post_clr(dst) atomic_clr_value(dst, 1)
#elif defined(__GNUG__)
#define atomic_post_clr(dst) __sync_val_compare_and_swap((dst), 1, 0)
#else
#error "atomic post clear operation not defined"
#endif

#endif  // SRC_COMPONENTS_INCLUDE_UTILS_ATOMIC_H_
