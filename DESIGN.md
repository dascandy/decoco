# Design choices

## Find\* functions using a string rather than an enum

They are functions that allow you to find a compressor or decompressor at runtime based on a runtime-only identifier. These identifiers are typically passed in through external interfaces or configuration files. Typical coding behavior would make an enum based interface have a translation at this layer, which completely prevents the forward-compatibility of adding new compressors from propagating to these configuration files or external interfaces. Using a string, despite being usually an anti-pattern, typically would not be translated or checked at these boundaries, relying on the library doing its job and allowing the future extensibility to work.

## unique\_ptr encapsulating a polymorphic Compressor type with hidden implementation

One of the design goals for the library is to provide a tiny header, both in direct inclusion size and indirect inclusion size, that provides a variety of compression methods. Exposing the implementation type would either require implementing a local\_ptr style object or exposing the full headers of the implementation libraries.

A local\_ptr style object, which functions as a local fixed-size allocator for a polymorphic type, would allow hiding the implementation details at the expense of the object itself becoming noncopyable and immovable, and would still require indirection through the polymorphic type / interface to function. As the maximum size of the objects are then encapsulated in this local\_ptr instantiation, it would not be possible to add new implementations that require a larger state without incurring an allocation at that point anyway, while at the same time requiring the smallest local types to still be allocated at the larger size. Its added complexity offers very little value.

Exposing the implementation of the specific compressors would have the advantage of being able to avoid the polymorphic indirection, stack-allocating them and potentially fully inlining them. It is not for free though; it drags in the full definition of all possible compressors supported, it requires development headers for all to be installed (rather than just decoco's header), it expands the view of the compiler enormously slowing it down and giving any tools analyzing the code much more to work with, it fills tools like You-Complete-Me and Intellisense with loads of definitions that are not desired; it creates a lot of macro definitions in the user's space that are unwanted and it makes it hard to keep any promise of compile time for the future.

## Allocations for decompressing and compressing data

While the compressor and decompressor themselves are a single allocation, the actual operations will return a new vector every time. Sadly, this implies that each compression or decompression call will perform one or multiple allocations per invocation. There are not too many alternatives to this:

- Pass in the buffer to use for data output
- Retain ownership of the returned vector and reuse it for a subsequent call

Passing in the buffer to use would transform the interface to one where the caller determines the space to be used. This might be a good idea eventually; I'm not ruling this one out yet. It does change the function design to one where a parameter is used for the return value.

Retaining ownership of the vector that is returned works badly as the user is not informed that the lifetime of the vector, and the lifetime of the data, are both unclear from the function invocation. This wreaks havoc on the ability to debug any such problems, and as such I don't consider this a worthwhile idea to explore.


